#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "ast/NumberNode.h"

#include "IRRenderer.h"


llvm::Value *
IRRenderer::render_node(NumberNode *node) {
  auto &context = get_render_context();
  auto &llvm_context = context.get_llvm_context();

  return llvm::ConstantFP::get(llvm_context, llvm::APFloat(node->val));
}
