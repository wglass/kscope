#include "llvm/IR/Value.h"

#include "ast/variable.h"
#include "context.h"
#include "errors.h"


llvm::Value *
VariableNode::codegen(Context *context) {
    llvm::Value *val = context->get_named_value(name);
    if ( !val ) {
        return ErrorV("Unknown variable name");
    }

    return context->builder()->CreateLoad(val, name.c_str());
}
