%skeleton "lalr1.cc"
%require "3.0"

%verbose

%defines
%define parser_class_name {Parser}
%define api.namespace {bison}

%code requires{
    #include "ast/ASTNode.h"
    #include "ast/BinaryNode.h"
    #include "ast/CallNode.h"
    #include "ast/ForNode.h"
    #include "ast/FunctionNode.h"
    #include "ast/IfNode.h"
    #include "ast/NumberNode.h"
    #include "ast/PrototypeNode.h"
    #include "ast/UnaryNode.h"
    #include "ast/VarNode.h"
    #include "ast/VariableNode.h"

    #include "ASTree.h"

    class Lexer;
    class ASTree;

    typedef std::vector<std::unique_ptr<ASTNode>> NodeVector;
    typedef std::pair<std::string, std::unique_ptr<ASTNode>> NodePair;
}

%param { Lexer &lexer }
%parse-param { ASTree &tree }

%code{
static int yylex(bison::Parser::semantic_type *yylval,
                 Lexer &lexer);
}

%union {
    double num;
    char chr;

    std::string * str;
    std::unique_ptr<ASTNode> node;
    std::unique_ptr<PrototypeNode> proto;
    std::unique_ptr<FunctionNode> func;
    std::unique_ptr<std::vector<std::string>> strs;
    std::unique_ptr<NodeVector> nodes;
    std::unique_ptr<NodePair> declr;
    std::unique_ptr<std::vector<NodePair>> declrs;
}

%destructor {
    if ($$)  { $$.release(); }
} <node> <proto> <func> <strs> <nodes> <declr> <declrs>
%destructor {
    if ($$)  { delete ($$); ($$) = nullptr; }
} <str>

%define api.token.prefix {}

%token END 0
%token DEF "def"
%token EXTERN "extern"
%token <str> IDENTIFIER
%token <num> NUMBER

%token <chr> ASSIGNMENT "="

%token <chr> MULTIPLY "*"
%token <chr> DIVIDE "/"
%token <chr> ADD "+"
%token <chr> SUBTRACT "-"
%token <chr> GREATER_THAN ">"
%token <chr> LESS_THAN "<"

%token IF "if"
%token THEN "then"
%token ELSE "else"
%token FOR "for"
%token IN "in"
%token VAR "var"

%token OPEN_PAREN "("
%token CLOSE_PAREN ")"
%token STATEMENT_END ";"
%token COMMA ","

%type <node> expr number_literal binary_op call variable
%type <nodes> call_args
%type <node> if_then for_loop var_declare
%type <proto> prototype extern
%type <strs> arg_names
%type <func> definition
%type <declrs> declarations
%type <declr> declaration

%%
%start top;

top :
  definition END { tree.set_root(std::move($1)); }
| extern END { tree.set_root(std::move($1)); }
| expr END {
    auto proto = std::make_unique<PrototypeNode>("", std::vector<std::string>());
    tree.set_root(std::make_unique<FunctionNode>(proto, $1));
}

expr :
  binary_op
| call
| if_then
| for_loop
| var_declare
| variable
| number_literal
| "(" expr ")" { $$ = std::move($2); }

variable:
  IDENTIFIER { $$ = std::make_unique<VariableNode>(*$1); }

number_literal :
  NUMBER { $$ = std::make_unique<NumberNode>($1); }

%left "=";
%left ">" "<";
%left "+" "-";
%left "*" "/";

binary_op :
  expr "=" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr "+" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr "-" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr "*" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr "/" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr "<" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }
| expr ">" expr { $$ = std::make_unique<BinaryNode>(std::move($2), $1, std::move($3)); }

call :
IDENTIFIER "(" call_args ")" {
  $$ = std::make_unique<CallNode>(*$1, std::move($3));
}

call_args :
  { $$ = std::make_unique<NodeVector>(); }
| call_args "," expr {
    $$ = std::move($1);
    $$->push_back(std::move($3));
  }
| expr {
    $$ = std::make_unique<NodeVector>();
    $$->push_back(std::move($1));
  }

extern :
"extern" prototype { $$ = std::move($2); }

definition :
"def" prototype expr {
    $$ = std::make_unique<FunctionNode>(std::move($2), std::move($3));
}

prototype :
IDENTIFIER "(" arg_names ")" {
    $$ = std::make_unique<PrototypeNode>(*$1, std::move($3));
}

arg_names:
  { $$ = std::make_unique<std::vector<std::string>>(); }
| arg_names "," IDENTIFIER {
    $$ = std::move($1);
    $$->push_back($3);
  }
| IDENTIFIER {
    $$ = std::make_unique<std::vector<std::string>>();
    $$->push_back($1);
  }

%left "else" "then";
%right "," "in";

if_then :
  "if" expr "then" expr "else" expr {
    $$ = std::make_unique<IfNode>(std::move($2), std::move($4), std::move($6));
  }

for_loop :
  "for" IDENTIFIER "=" expr "," expr "in" expr {
    $$ = std::make_unique<ForNode>(*$2, std::move($4), std::move($6), nullptr, std::move($8));
  }
| "for" IDENTIFIER "=" expr "," expr "," expr "in" expr {
    $$ = std::make_unique<ForNode>(*$2, std::move($4), std::move($6), std::move($8), std::move($10));
  }


var_declare :
  "var" declarations "in" expr {
    $$ = std::make_unique<VarNode>(std::move($2), std::move($4));
  }

declarations :
  {
    $$ = std::make_unique<std::vector<NodePair>>();
  }
| declarations "," declaration {
    $$ = std::move($1);
    $$->push_back(std::move($3));
  }
| declaration {
    $$ = std::make_unique<std::vector<NodePair>>();
    $$->push_back(std::move($1));
  }


declaration :
  IDENTIFIER "=" expr {
    $$ = std::make_unique<NodePair>($1, std::move($3));
  }
| IDENTIFIER {
    $$ = std::make_unique<NodePair>($1, nullptr);
  }

%%
#include "lexer.h"

static int yylex(bison::Parser::semantic_type *yylval,
                 Lexer &lexer) {
    return lexer.yylex(yylval);
}

void
bison::Parser::error( const std::string &err_message )
{
   std::cerr << "Error: " << err_message << "\n";
}
