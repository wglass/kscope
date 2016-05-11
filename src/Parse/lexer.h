#pragma once

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef  YY_DECL
#define YY_DECL int Lexer::yylex()

#include "bison_parser.hh"


class Lexer : public yyFlexLexer {

public:
  Lexer(std::istream *in);

  int yylex(bison::Parser::semantic_type *l_val);

private:
  Lexer(Lexer &other) = delete;
  int yylex();
  bison::Parser::semantic_type *yylval;
};
