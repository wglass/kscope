#include <cstdio>
#include <string>
#include <map>

#include "lexer.h"


Lexer::Lexer() {
    op_precedence_['='] = 2;
    op_precedence_['<'] = 10;
    op_precedence_['>'] = 10;
    op_precedence_['+'] = 20;
    op_precedence_['-'] = 20;
    op_precedence_['*'] = 240;

    keyword_map_["def"] = Token::DEF;
    keyword_map_["extern"] = Token::EXTERN;
    keyword_map_["var"] = Token::VAR;
    keyword_map_["if"] = Token::IF;
    keyword_map_["then"] = Token::THEN;
    keyword_map_["else"] = Token::ELSE;
    keyword_map_["for"] = Token::FOR;
    keyword_map_["in"] = Token::IN;
}

Lexer::Lexer(const Lexer &other)
    : current_token_(other.current_token()),
      identifier_(other.identifier()),
      number_value_(other.number_value()) {

    op_precedence_['='] = 2;
    op_precedence_['<'] = 10;
    op_precedence_['>'] = 10;
    op_precedence_['+'] = 20;
    op_precedence_['-'] = 20;
    op_precedence_['*'] = 240;

    keyword_map_["def"] = Token::DEF;
    keyword_map_["extern"] = Token::EXTERN;
    keyword_map_["var"] = Token::VAR;
    keyword_map_["if"] = Token::IF;
    keyword_map_["then"] = Token::THEN;
    keyword_map_["else"] = Token::ELSE;
    keyword_map_["for"] = Token::FOR;
    keyword_map_["in"] = Token::IN;
}

void
Lexer::next() {
    static int last_char = ' ';

    while ( isspace(last_char) ) {
        last_char = getchar();
    }

    if ( isalpha(last_char) ) {
        identifier_ = last_char;
        while ( isalnum((last_char = getchar())) ) {
            identifier_ += last_char;
        }

        if ( keyword_map_.count(identifier_) == 0 ) {
            current_token_ = Token::IDENTIFIER;
            return;
        } else {
            current_token_ = keyword_map_[identifier_];
            return;
        }
    }

    if ( isdigit(last_char) || last_char == '.' ) {
        std::string number_string;

        do {
            number_string += last_char;
            last_char = getchar();
        } while ( isdigit(last_char) || last_char == '.' );

        number_value_ = strtod(number_string.c_str(), 0);

        current_token_ = Token::NUMBER;
        return;
    }

    if ( last_char == '#' ) {
        do {
            last_char = getchar();
        } while ( last_char != EOF && last_char != '\n' && last_char != '\r' );

        if ( last_char != EOF ) {
            next();
            return;
        }
    }

    if ( last_char == EOF ) {
        current_token_ = Token::_EOF;
        return;
    }

    current_token_ = last_char;
    last_char = getchar();
}

const int
Lexer::current_token() const { return current_token_; }

const std::string
Lexer::identifier() const { return identifier_; }

const double
Lexer::number_value() const { return number_value_; }

int
Lexer::token_precedence() {
    if ( !isascii(current_token_) ) {
        return -1;
    }

    int precedence = op_precedence_[current_token_];
    if ( precedence <= 0 ) {
        return -1;
    }

    return precedence;
}

void
Lexer::add_op_precedence(const char &op, const int precedence) {
    op_precedence_[op] = precedence;
}

const std::map<char, int>
Lexer::op_precedence() const {
    return op_precedence_;
}
