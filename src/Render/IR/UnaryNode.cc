#include "IRRenderer.h"

#include "kscope/AST/UnaryNode.h"
#include "kscope/Errors/Error.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"


llvm::Value *
IRRenderer::visit_node(UnaryNode *node) {
  auto &context = get_render_context();
  auto &module = context.get_module();
  auto &builder = context.get_builder();

  auto *operand_value = visit(node->operand);
  if ( operand_value == 0 ) { return nullptr; }

  auto *func = module.getFunction(std::string("unary") + node->opcode);
  if ( func == 0 ) {
    return Error<llvm::Value>::handle("Unknown unary operator");
  }

  return builder.CreateCall(func, operand_value, "unop");
}
