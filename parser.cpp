
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "errors.h"

ASTNode *ParseNumber() {
    ASTNode *Result = new NumberNode(NumVal);
    getNextToken();
    return Result;
}

ASTNode *ParseParen() {
    getNextToken();
    ASTNode *V = ParseExpression();
    if ( !V ) {
        return 0;
    }

    if ( current_token != ')' ) {
        return Error("expected ')'");
    }
    getNextToken();
    return V;
}

ASTNode *ParseIdentifier() {
    std::string IdName = IdentifierStr;

    getNextToken();

    if ( current_token != '(' ) {
        return new VariableNode(IdName);
    }

    getNextToken();
    std::vector<ASTNode*> Args;
    if ( current_token != ')' ) {
        while (1) {
            ASTNode *Arg = ParseExpression();
            if ( !Arg ) {
                return 0;
            }

            Args.push_back(Arg);

            if ( current_token == ')') {
                break;
            }

            if ( current_token != ',' ) {
                return Error("expected ')' or ',' in argument list");
            }

            getNextToken();
        }
    }
    getNextToken();

    return new CallNode(IdName, Args);
}

ASTNode *ParseVar() {
    getNextToken();

    std::vector<std::pair<std::string, ASTNode*> > var_names;

    if ( current_token != tok_identifier ) {
        return Error("Expected identifier after var");
    }

    while (1) {
        std::string name = IdentifierStr;
        getNextToken();

        ASTNode *init = 0;
        if ( current_token == '=' ) {
            getNextToken();
            init = ParseExpression();
            if ( init == 0 ) { return 0; }
        }

        var_names.push_back(std::make_pair(name, init));

        if ( current_token != ',') { break; }

        getNextToken();

        if ( current_token != tok_identifier ) {
            return Error("Expected identifier list after var");
        }
    }

    if ( current_token != tok_in ) {
        return Error("Expected 'in' keyword after 'var'");
    }
    getNextToken();

    ASTNode *body = ParseExpression();
    if ( body == 0 ) { return 0; }

    return new VarNode(var_names, body);
}

ASTNode *ParsePrimary() {
    switch( current_token ) {
    default:
        return Error("unknown token when expecting an expression");
    case tok_identifier: return ParseIdentifier();
    case tok_number: return ParseNumber();
    case '(': return ParseParen();
    case tok_if: return ParseIf();
    case tok_for: return ParseFor();
    case tok_var: return ParseVar();
    }
}

ASTNode *ParseBinOpRHS(int expression_precedence, ASTNode *LHS) {
    while (1) {
        int token_precedence = getTokPrecedence();
        if ( token_precedence < expression_precedence ) {
            return LHS;
        }

        int BinOp = current_token;
        getNextToken();

        ASTNode *RHS = ParseUnary();
        if ( !RHS ) { return 0; }

        int next_precedence = getTokPrecedence();
        if ( token_precedence < next_precedence ) {
            RHS = ParseBinOpRHS(token_precedence + 1, RHS);
        }

        LHS = new BinaryNode(BinOp, LHS, RHS);
    }
}

ASTNode *ParseUnary() {
    if ( !isascii(current_token) || current_token == '(' || current_token == ')' ) {
        return ParsePrimary();
    }

    int opr = current_token;
    getNextToken();
    if ( ASTNode *operand = ParseUnary() ) {
        return new UnaryNode(opr, operand);
    }

    return 0;
}

ASTNode *ParseExpression() {
    ASTNode *LHS = ParseUnary();
    if ( !LHS ) { return 0; }

    return ParseBinOpRHS(0, LHS);
}

PrototypeNode *ParsePrototype() {
    std::string func_name;

    unsigned kind = 0;
    unsigned binary_precedence = 0;

    switch(current_token) {
    case tok_identifier:
        kind = 0;
        func_name = IdentifierStr;
        getNextToken();
        break;
    case tok_unary:
        getNextToken();
        if ( ! isascii(current_token) ) {
            return ErrorP("Expected unary operator");
        }
        kind = 1;
        func_name = "unary";
        func_name += (char)current_token;
        getNextToken();
        break;
    case tok_binary:
        getNextToken();
        if ( ! isascii(current_token) ) {
            return ErrorP("Expected binary operator");
        }
        kind = 2;
        func_name = "binary";
        func_name += (char)current_token;
        getNextToken();
        if ( current_token == tok_number ) {
            if ( NumVal < 1 || NumVal > 100 ) {
                return ErrorP("Invalid precedence: must be 1..100");
            }
            binary_precedence = (unsigned)NumVal;
            getNextToken();
        }
        break;
    }

    if ( current_token != '(' ) {
        return ErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> arg_names;

    while ( getNextToken() == tok_identifier ) {
        arg_names.push_back(IdentifierStr);
    }

    if ( current_token != ')' ) {
        return ErrorP("Expected ')' in prototype");
    }

    getNextToken();

    if ( kind && arg_names.size() != kind ) {
        return ErrorP("Invalid number of operands for operator");
    }

    return new PrototypeNode(func_name, arg_names, kind != 0, binary_precedence);
}

FunctionNode *ParseDefinition() {
    getNextToken();

    PrototypeNode *Proto = ParsePrototype();

    if ( Proto == 0 ) { return 0; }

    if ( ASTNode *E = ParseExpression() ) {
        return new FunctionNode(Proto, E);
    }

    return 0;
}

PrototypeNode *ParseExtern() {
    getNextToken();

    return ParsePrototype();
}

FunctionNode *ParseTopLevelExpression() {
    if ( ASTNode *E = ParseExpression() ) {
        PrototypeNode *Proto = new PrototypeNode("", std::vector<std::string>());
        return new FunctionNode(Proto, E);
    }

    return 0;
}

ASTNode *ParseIf() {
    getNextToken();

    ASTNode *condition = ParseExpression();
    if ( ! condition ) { return 0; }

    if ( current_token != tok_then ) {
        return Error("expected 'then'");
    }

    getNextToken();

    ASTNode *then = ParseExpression();
    if ( ! then ) { return 0; }

    if ( current_token != tok_else ) {
        return Error("expected 'else'");
    }

    getNextToken();

    ASTNode *_else = ParseExpression();
    if ( ! _else ) { return 0; }

    return new IfNode(condition, then, _else);
}

ASTNode *ParseFor() {
    getNextToken();

    if ( current_token != tok_identifier ) {
        return Error("expected identifier after 'for'");
    }

    std::string id_name = IdentifierStr;
    getNextToken();

    if ( current_token != '=' ) {
        return Error("expected '=' after 'for'");
    }
    getNextToken();

    ASTNode *start = ParseExpression();
    if ( start == 0 ) { return 0; }

    if ( current_token != ',' ) {
        return Error("Expected ',' after for start value");
    }
    getNextToken();

    ASTNode *end = ParseExpression();
    if ( end == 0 ) { return 0; }

    ASTNode *step = 0;
    if ( current_token == ',' ) {
        getNextToken();
        step = ParseExpression();
        if ( step == 0 ) { return 0; }
    }

    if ( current_token != tok_in ) {
        return Error("Expected 'in' after 'for'");
    }
    getNextToken();

    ASTNode *body = ParseExpression();
    if ( body == 0 ) { return 0; }

    return new ForNode(id_name, start, end, step, body);
}
