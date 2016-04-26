#include "IRRenderer.h"

#include "ast/ASTNode.h"
#include "ast/VarNode.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include <cstddef>
#include <string>
#include <vector>


template <class Pipeline>
llvm::Value *
IRRenderer<Pipeline>::render_node(VarNode *node) {
  auto &context = get_render_context();
  auto &llvm_context = context.get_llvm_context();
  auto &builder = context.get_builder();

  std::vector<llvm::AllocaInst *> old_bindings;

  auto zero = llvm::ConstantFP::get(llvm_context, llvm::APFloat(0.0));

  llvm::Function *func = builder.GetInsertBlock()->getParent();

  for ( auto &var_pair : node->var_names ) {
    const std::string &var_name = var_pair.first;
    ASTNode *init = var_pair.second;

    llvm::Value *init_val;
    if ( init ) {
      init_val = render(init);
      if ( init_val == 0 ) { return nullptr; }
    } else {
      init_val = zero;
    }

    llvm::AllocaInst *alloca = context.create_entry_block_alloca(func, var_name);
    builder.CreateStore(init_val, alloca);

    old_bindings.push_back(context.get_named_value(var_name));
    context.set_named_value(var_name, alloca);
  }

  llvm::Value *body_val = render(node->body);
  if ( body_val == 0 ) { return nullptr; }

  std::size_t index = 0;
  for ( auto &var_pair : node->var_names ) {
    context.set_named_value(var_pair.first, old_bindings[index]);
    ++index;
  }

  return body_val;
}
