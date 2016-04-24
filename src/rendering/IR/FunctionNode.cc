#include "IRRenderer.h"

#include "ast/FunctionNode.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"


llvm::Function *
IRRenderer::render_node(FunctionNode *node) {
  pipeline->add_function(node);
  return nullptr;
}


llvm::Function *
IRRenderer::render_function(FunctionNode *node) {
  auto &context = get_render_context();
  auto &builder = context.get_builder();
  auto &pass_manager = context.get_pass_manager();

  context.clear_all_named_values();

  llvm::Function *func = render_node(node->proto);
  if ( func == 0 ) { return nullptr; }

  llvm::BasicBlock *block = llvm::BasicBlock::Create(context.get_llvm_context(),
                                                     "entry",
                                                     func);
  builder.SetInsertPoint(block);

  context.create_argument_allocas(func, node->proto->args);

  llvm::Value *retval = render(node->body);

  if ( ! retval ) {
    func->eraseFromParent();
    return nullptr;
  }

  builder.CreateRet(retval);
  llvm::verifyFunction(*func);

  pass_manager.run(*func);

  return func;
}
