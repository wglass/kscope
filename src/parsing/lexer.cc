#include "lexer.h"
#include "bison_parser.hh"


Lexer::Lexer(std::istream *in)
    : yyFlexLexer(in), yylval(nullptr) {}

int
Lexer::yylex(bison::Parser::semantic_type *l_val) {
    yylval = l_val;
    return yylex();
}
