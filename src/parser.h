#pragma once

#include "ast/node.h"
#include "ast/prototype.h"
#include "ast/function.h"


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
