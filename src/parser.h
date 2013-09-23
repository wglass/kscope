#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include "ast/ast.h"

ASTNode *ParseExpression();
ASTNode *ParseIf();
ASTNode *ParseFor();
ASTNode *ParseUnary();
ASTNode *ParseNumber();
ASTNode *ParseParen();
ASTNode *ParseIdentifier();
ASTNode *ParseVar();
ASTNode *ParsePrimary();
ASTNode *ParseBinOpRHS();
PrototypeNode *ParsePrototype();
FunctionNode *ParseDefinition();
PrototypeNode *ParseExtern();
FunctionNode *ParseTopLevelExpression();

#endif
