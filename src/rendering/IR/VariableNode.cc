#include "llvm/IR/Value.h"

#include "ast/VariableNode.h"
#include "errors/Error.h"

#include "IRRenderer.h"


llvm::Value *
IRRenderer::render(VariableNode *node) {
    llvm::Value *val = get_named_value(node->name);
    if ( !val ) {
      return Error<llvm::Value>::handle("Unknown variable name");
    }

    return builder->CreateLoad(val, node->name.c_str());
}
