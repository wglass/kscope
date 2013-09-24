#pragma once

#include "llvm/IR/Value.h"

#include "ast/node.h"
#include "ast/prototype.h"
#include "ast/function.h"


ASTNode *Error(const char *message);
PrototypeNode *ErrorP(const char *message);
FunctionNode *ErrorF(const char *message);
llvm::Value *ErrorV(const char *message);
