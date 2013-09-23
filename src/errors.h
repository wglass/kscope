#ifndef __ERRORS_H_INCLUDED__
#define __ERRORS_H_INCLUDED__

#include "llvm/IR/Value.h"

#include "ast.h"

ASTNode *Error(const char *message);
PrototypeNode *ErrorP(const char *message);
FunctionNode *ErrorF(const char *message);
llvm::Value *ErrorV(const char *message);


#endif
