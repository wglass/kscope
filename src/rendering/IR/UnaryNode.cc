#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/UnaryNode.h"
#include "errors/Error.h"

#include "IRRenderer.h"


llvm::Value *
IRRenderer::render_node(UnaryNode *node) {
  auto &context = get_render_context();
  auto &module = context.get_module();
  auto &builder = context.get_builder();

  llvm::Value *operand_value = render(node->operand);
  if ( operand_value == 0 ) { return nullptr; }

  llvm::Function *func = module.getFunction(std::string("unary") + node->opcode);
  if ( func == 0 ) {
    return Error<llvm::Value>::handle("Unknown unary operator");
  }

  return builder.CreateCall(func, operand_value, "unop");
}
