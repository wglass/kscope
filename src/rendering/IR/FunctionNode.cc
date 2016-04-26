#include "IRRenderer.h"

#include "ast/FunctionNode.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include <iostream>


llvm::Function *
IRRenderer::render_node(FunctionNode *node) {
  fprintf(stderr, "Adding function %s to pipeline.\n", node->proto->name.c_str());
  pipeline->process_function_node(node);
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

  fprintf(stderr, "Read function definition.");
  func->dump();

  return func;
}
