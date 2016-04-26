#include "IRRenderer.h"

#include "ast/VariableNode.h"
#include "errors/Error.h"

#include "llvm/IR/Value.h"


template <class Pipeline>
llvm::Value *
IRRenderer<Pipeline>::render_node(VariableNode *node) {
  auto &context = get_render_context();

  llvm::Value *val = context.get_named_value(node->name);
  if ( !val ) {
    return Error<llvm::Value>::handle("Unknown variable name");
  }

  return context.get_builder().CreateLoad(val, node->name.c_str());
}
