#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "errors.h"

#include <cstdio>


Parser::Parser()
    : lexer_(new Lexer()) {}

Parser::Parser(const Parser &other)
    : lexer_(new Lexer()) {}


Parser &
Parser::operator =(const Parser &other) {
    if ( this == &other ) { return *this; }

    delete lexer_;

    lexer_ = new Lexer();

    return *this;
}

Parser::~Parser() {
    delete lexer_;
}

Lexer *
Parser::lexer() { return lexer_; }

ASTNode *
Parser::parse_number() {
    ASTNode *result = new NumberNode(lexer_->number_value());
    lexer_->next();
    return result;
}

ASTNode *
Parser::parse_paren() {
    lexer_->next();

    ASTNode *value = parse_expression();
    if ( !value ) {
        return 0;
    }
    if ( lexer_->current_token() != ')' ) {
        return Error("expected ')'");
    }

    lexer_->next();
    return value;
}

ASTNode *
Parser::parse_identifier() {
    std::string identifier_name = lexer_->identifier();

    lexer_->next();

    if ( lexer_->current_token() != '(' ) {
        return new VariableNode(identifier_name);
    }

    lexer_->next();

    std::vector<ASTNode*> args;
    if ( lexer_->current_token() != ')' ) {
        while (1) {
            ASTNode *arg = parse_expression();
            if ( !arg ) {
                return 0;
            }

            args.push_back(arg);

            if ( lexer_->current_token() == ')' ) {
                break;
            }

            if ( lexer_->current_token() != ',' ) {
                return Error("expected ')' or ',' in argument list");
            }

            lexer_->next();
        }
    }

    lexer_->next();

    return new CallNode(identifier_name, args);
}

ASTNode *
Parser::parse_var() {
    lexer_->next();

    std::vector<std::pair<std::string, ASTNode*> > var_names;

    if ( lexer_->current_token() != Token::IDENTIFIER ) {
        return Error("Expected identifier after var");
    }

    while (1) {
        std::string name = lexer_->identifier();
        lexer_->next();

        ASTNode *init = 0;
        if ( lexer_->current_token() == '=' ) {
            lexer_->next();
            init = parse_expression();
            if ( init == 0 ) { return 0; }
        }

        var_names.push_back(std::make_pair(name, init));

        if ( lexer_->current_token() != ',') { break; }

        lexer_->next();

        if ( lexer_->current_token() != Token::IDENTIFIER ) {
            return Error("Expected identifier list after var");
        }
    }

    if ( lexer_->current_token() != Token::IN ) {
        return Error("Expected 'in' keyword after 'var'");
    }
    lexer_->next();

    ASTNode *body = parse_expression();
    if ( body == 0 ) { return 0; }

    return new VarNode(var_names, body);
}

ASTNode *
Parser::parse_primary() {
    switch( lexer_->current_token() ) {
    case Token::IDENTIFIER:
        return parse_identifier();
    case Token::NUMBER:
        return parse_number();
    case '(':
        return parse_paren();
    case Token::IF:
        return parse_if();
    case Token::FOR:
        return parse_for();
    case Token::VAR:
        return parse_var();
    default:
        fprintf(stderr, "Unknown token '%c' when expecting an expression", (char) lexer_->current_token());
        return 0;
    }
}

ASTNode *
Parser::parse_binary_op_rhs(int expression_precedence, ASTNode *LHS) {
    while (1) {
        int token_precedence = lexer_->token_precedence();
        if ( token_precedence < expression_precedence ) {
            return LHS;
        }

        int binary_op = lexer_->current_token();
        lexer_->next();

        ASTNode *RHS = parse_unary();
        if ( !RHS ) { return 0; }

        int next_precedence = lexer_->token_precedence();
        if ( token_precedence < next_precedence ) {
            RHS = parse_binary_op_rhs(token_precedence + 1, RHS);
        }

        LHS = new BinaryNode(binary_op, LHS, RHS);
    }
}

ASTNode *
Parser::parse_unary() {
    if ( !isascii(lexer_->current_token())
         || lexer_->current_token() == '('
         || lexer_->current_token() == ')' ) {
        return parse_primary();
    }

    int unary_op = lexer_->current_token();
    lexer_->next();

    if ( ASTNode *operand = parse_unary() ) {
        return new UnaryNode(unary_op, operand);
    }

    return 0;
}

ASTNode *
Parser::parse_expression() {
    ASTNode *LHS = parse_unary();
    if ( !LHS ) { return 0; }

    return parse_binary_op_rhs(0, LHS);
}

PrototypeNode *
Parser::parse_prototype() {
    std::string func_name;

    if ( lexer_->current_token() == Token::IDENTIFIER ) {
        func_name = lexer_->identifier();
        lexer_->next();
    }

    if ( lexer_->current_token() != '(' ) {
        return ErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> arg_names;

    lexer_->next();

    while ( lexer_->current_token() == Token::IDENTIFIER ) {
        arg_names.push_back(lexer_->identifier());
        lexer_->next();
    }

    if ( lexer_->current_token() != ')' ) {
        return ErrorP("Expected ')' in prototype");
    }

    lexer_->next();

    return new PrototypeNode(func_name, arg_names);
}

FunctionNode *
Parser::parse_definition() {
    lexer_->next();

    PrototypeNode *proto = parse_prototype();

    if ( proto == 0 ) { return 0; }

    if ( ASTNode *expression = parse_expression() ) {
        return new FunctionNode(proto, expression);
    }

    return 0;
}

PrototypeNode *
Parser::parse_extern() {
    lexer_->next();

    return parse_prototype();
}

FunctionNode *
Parser::parse_top_level_expression() {
    if ( ASTNode *expression = parse_expression() ) {
        PrototypeNode *proto = new PrototypeNode("", std::vector<std::string>());
        return new FunctionNode(proto, expression);
    }

    return 0;
}

ASTNode *
Parser::parse_if() {
    lexer_->next();

    ASTNode *condition = parse_expression();
    if ( ! condition ) { return 0; }

    if ( lexer_->current_token() != Token::THEN ) {
        return Error("expected 'then'");
    }

    lexer_->next();

    ASTNode *then = parse_expression();
    if ( ! then ) { return 0; }

    if ( lexer_->current_token() != Token::ELSE ) {
        return Error("expected 'else'");
    }

    lexer_->next();

    ASTNode *_else = parse_expression();
    if ( ! _else ) { return 0; }

    return new IfNode(condition, then, _else);
}

ASTNode *
Parser::parse_for() {
    lexer_->next();

    if ( lexer_->current_token() != Token::IDENTIFIER ) {
        return Error("expected identifier after 'for'");
    }

    std::string id_name = lexer_->identifier();
    lexer_->next();

    if ( lexer_->current_token() != '=' ) {
        return Error("expected '=' after 'for'");
    }
    lexer_->next();

    ASTNode *start = parse_expression();
    if ( start == 0 ) { return 0; }

    if ( lexer_->current_token() != ',' ) {
        return Error("Expected ',' after for start value");
    }
    lexer_->next();

    ASTNode *end = parse_expression();
    if ( end == 0 ) { return 0; }

    ASTNode *step = 0;
    if ( lexer_->current_token() == ',' ) {
        lexer_->next();
        step = parse_expression();
        if ( step == 0 ) { return 0; }
    }

    if ( lexer_->current_token() != Token::IN ) {
        return Error("Expected 'in' after 'for'");
    }
    lexer_->next();

    ASTNode *body = parse_expression();
    if ( body == 0 ) { return 0; }

    return new ForNode(id_name, start, end, step, body);
}
