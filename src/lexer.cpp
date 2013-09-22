#include <cstdio>
#include <string>
#include <map>

#include "lexer.h"

int current_token;
std::string IdentifierStr;
double NumVal;
std::map<char, int> op_precedence;

static int gettok() {
    static int last_char = ' ';

    while ( isspace(last_char) ) {
        last_char = getchar();
    }

    if ( isalpha(last_char) ) {
        IdentifierStr = last_char;
        while ( isalnum((last_char = getchar())) ) {
            IdentifierStr += last_char;
        }

        if ( IdentifierStr == "def" ) {
            return tok_def;
        }
        if ( IdentifierStr == "extern" ) {
            return tok_extern;
        }
        if ( IdentifierStr == "if" ) {
            return tok_if;
        }
        if ( IdentifierStr == "then" ) {
            return tok_then;
        }
        if ( IdentifierStr == "else" ) {
            return tok_else;
        }
        if ( IdentifierStr == "for" ) {
            return tok_for;
        }
        if ( IdentifierStr == "in" ) {
            return tok_in;
        }
        if ( IdentifierStr == "binary" ) {
            return tok_binary;
        }
        if ( IdentifierStr == "unary" ) {
            return tok_unary;
        }
        if ( IdentifierStr == "var" ) {
            return tok_var;
        }

        return tok_identifier;
    }

    if ( isdigit(last_char) || last_char == '.' ) {
        std::string number_string;

        do {
            number_string += last_char;
            last_char = getchar();
        } while ( isdigit(last_char) || last_char == '.' );

        NumVal = strtod(number_string.c_str(), 0);
        return tok_number;
    }

    if ( last_char == '#' ) {
        do {
            last_char = getchar();
        } while ( last_char != EOF && last_char != '\n' && last_char != '\r' );

        if ( last_char != EOF ) {
            return gettok();
        }
    }

    if ( last_char == EOF ) {
        return tok_eof;
    }

    int this_char = last_char;
    last_char = getchar();

    return this_char;
}

int getNextToken() {
    return current_token = gettok();
}

int getTokPrecedence() {
    if ( !isascii(current_token) ) {
        return -1;
    }

    int tokPrec = op_precedence[current_token];
    if ( tokPrec <= 0 ) {
        return -1;
    }
    return tokPrec;
}
