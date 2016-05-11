#include "IRRenderer.h"

#include "kscope/AST/ForNode.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"


llvm::Value *
IRRenderer::render_node(ForNode *node) {
  auto &context = get_render_context();
  auto &llvm_context = context.get_llvm_context();
  auto &builder = context.get_builder();

  auto one = llvm::ConstantFP::get(llvm_context, llvm::APFloat(1.0));
  auto zero = llvm::ConstantFP::get(llvm_context, llvm::APFloat(0.0));

  llvm::Function *func = builder.GetInsertBlock()->getParent();

  llvm::AllocaInst *alloca = context.create_entry_block_alloca(func,
                                                               node->var_name);

  llvm::Value *start_value = render(node->start);
  if ( start_value == 0 ) { return nullptr; }

  builder.CreateStore(start_value, alloca);

  llvm::BasicBlock *loop_block = llvm::BasicBlock::Create(llvm_context,
                                                          "loop", func);

  builder.CreateBr(loop_block);
  builder.SetInsertPoint(loop_block);

  llvm::AllocaInst *old_value = context.get_named_value(node->var_name);
  context.set_named_value(node->var_name, alloca);

  if ( render(node->body) == 0 ) { return nullptr; }

  llvm::Value *step_value;
  if ( node->step ) {
    step_value = render(node->step);
    if ( step_value == 0 ) { return nullptr; }
  } else {
    step_value = one;
  }

  llvm::Value *end_condition = render(node->end);
  if ( end_condition == 0 ) { return nullptr; }

  llvm::Value *current_var = builder.CreateLoad(alloca, node->var_name.c_str());
  llvm::Value *next_var = builder.CreateFAdd(current_var, step_value, "nextvar");
  builder.CreateStore(next_var, alloca);

  end_condition = builder.CreateFCmpONE(end_condition, zero, "loopcond");

  llvm::BasicBlock *after_block = llvm::BasicBlock::Create(llvm_context,
                                                           "afterloop", func);

  builder.CreateCondBr(end_condition, loop_block, after_block);
  builder.SetInsertPoint(after_block);

  if ( old_value ) {
    context.set_named_value(node->var_name, old_value);
  } else {
    context.clear_named_value(node->var_name);
  }

  return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(llvm_context));
}
