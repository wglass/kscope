%{
#include "bison_parser.hh"
#include "lexer.h"

typedef bison::Parser::token token;

#define yyterminate() return( token::END )
%}

%s COMMENT

%option debug

%option c++
%option yyclass="Lexer"
%option noyywrap
%option outfile="flex_lexer.cc"

blank      [ \t]
identifier [a-zA-Z_][a-zA-Z_0-9]*
numeric    [0-9]+(\.[0-9][0-9]?)?

%%

{blank}+  /* skip whitespace */

"def"    return token::DEF;
"extern" return token::EXTERN;
"var"    return token::VAR;
"if"     return token::IF;
"then"   return token::THEN;
"else"   return token::ELSE;
"for"    return token::FOR;
"in"     return token::IN;

"(" return token::OPEN_PAREN;
")" return token::CLOSE_PAREN;
";" return token::STATEMENT_END;
"," return token::COMMA;

"=" return token::ASSIGNMENT;

"*" { yylval->chr = *yytext; return token::MULTIPLY; }
"/" { yylval->chr = *yytext; return token::DIVIDE; }
"+" { yylval->chr = *yytext; return token::ADD; }
"-" { yylval->chr = *yytext; return token::SUBTRACT; }
">" { yylval->chr = *yytext; return token::GREATER_THAN; }
"<" { yylval->chr = *yytext; return token::LESS_THAN; }

{identifier} {
    yylval->str = new std::string( yytext );
    return token::IDENTIFIER;
}

{numeric} {
    yylval->num = strtod(yytext, 0);
    return token::NUMBER;
}

"#" BEGIN(COMMENT);

<COMMENT>{
    [^\n]+  // eat eat eat
    \n   { yylineno++; BEGIN(INITIAL); }
}

.	printf("bad input character '%s' at line %d\n", yytext, yylineno);
