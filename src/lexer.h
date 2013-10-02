#pragma once

#include <string>
#include <map>

enum class Token {
    _EOF = -1,
    DEF = -2, EXTERN = -3,
    IDENTIFIER = -4,
    NUMBER = -5,
    IF = -6, THEN = -7, ELSE = -8,
    FOR = -9,
    IN = -10,
    VAR = -13
};

class Lexer {
    int current_token_;
    std::string identifier_;
    double number_value_;
    std::map<char, int> op_precedence_;
    std::map<std::string, Token> keyword_map_;

public:
    Lexer();
    Lexer(const Lexer &other);

    void next();

    int current_token() const;
    const std::string identifier() const;
    double number_value() const;
    int token_precedence();

    void add_op_precedence(const char &op, const int precedence);

    const std::map<char, int> op_precedence() const;
};
