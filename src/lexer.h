#pragma once

#include <string>
#include <map>

struct Token {
    static const int _EOF = -1;
    static const int DEF = -2;
    static const int EXTERN = -3;
    static const int IDENTIFIER = -4;
    static const int NUMBER = -5;
    static const int IF = -6;
    static const int THEN = -7;
    static const int ELSE = -8;
    static const int FOR = -9;
    static const int IN = -10;
    static const int VAR = -13;
};

class Lexer {
    int current_token_;
    std::string identifier_;
    double number_value_;
    std::map<char, int> op_precedence_;
    std::map<std::string, int> keyword_map_;

public:
    Lexer();
    Lexer(const Lexer &other);

    void next();

    const int current_token() const;
    const std::string identifier() const;
    const double number_value() const;
    int token_precedence();

    void add_op_precedence(const char &op, const int precedence);

    const std::map<char, int> op_precedence() const;
};
