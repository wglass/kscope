#include "llvm/IR/Value.h"

#include "ast/variable.h"
#include "renderer.h"
#include "errors.h"


llvm::Value *
VariableNode::codegen(IRRenderer *renderer) {
    llvm::Value *val = renderer->get_named_value(name);
    if ( !val ) {
        return ErrorV("Unknown variable name");
    }

    return renderer->builder->CreateLoad(val, name.c_str());
}
