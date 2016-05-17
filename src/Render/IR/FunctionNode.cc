#include "IRRenderer.h"

#include "kscope/AST/FunctionNode.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include <iostream>


llvm::Value *
IRRenderer::render_node(FunctionNode *node) {
  proto_map->insert(std::make_pair(node->proto->name, node->proto));
  pipeline->process_function_node(node);
  return nullptr;
}


llvm::Function *
IRRenderer::render_function(FunctionNode *node) {
  auto &context = get_render_context();
  auto &builder = context.get_builder();
  auto &pass_manager = context.get_pass_manager();

  context.clear_all_named_values();

  auto *func = static_cast<llvm::Function*>(render_node(node->proto));
  if ( func == 0 ) { return nullptr; }

  auto *block = llvm::BasicBlock::Create(context.get_llvm_context(),
                                         "entry",
                                         func);
  builder.SetInsertPoint(block);

  context.create_argument_allocas(func, node->proto->args);

  auto *retval = render(node->body);

  if ( ! retval ) {
    func->eraseFromParent();
    return nullptr;
  }

  builder.CreateRet(retval);
  llvm::verifyFunction(*func);

  pass_manager.run(*func);

  fprintf(stderr, "Generated function definition:\n");
  func->dump();

  return func;
}
