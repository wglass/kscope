#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/UnaryNode.h"
#include "errors/Error.h"

#include "IRRenderer.h"


llvm::Value *
IRRenderer::render(UnaryNode *node) {
  llvm::Value *operand_value = render(node->operand);
  if ( operand_value == 0 ) { return 0; }

  llvm::Function *func = module->getFunction(std::string("unary") + node->opcode);
  if ( func == 0 ) {
    return Error<llvm::Value>::handle("Unknown unary operator");
  }

  return builder->CreateCall(func, operand_value, "unop");
}
