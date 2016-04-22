#include "IRRenderer.h"

#include "ast/IfNode.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"


llvm::Value *
IRRenderer::render(IfNode *node) {
  auto &context = get_render_context();
  auto &builder = context.get_builder();

  auto zero = llvm::ConstantFP::get(llvm_context, llvm::APFloat(0.0));

  llvm::Value *cond_value = render(node->condition);
  if ( cond_value == 0 ) { return nullptr; }

  cond_value = builder.CreateFCmpONE(cond_value, zero, "ifcond");

  llvm::Function *func = builder.GetInsertBlock()->getParent();

  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(llvm_context, "then",
                                                          func);
  llvm::BasicBlock *else_block = llvm::BasicBlock::Create(llvm_context, "else");
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(llvm_context, "ifcont");

  builder.CreateCondBr(cond_value, then_block, else_block);
  builder.SetInsertPoint(then_block);

  llvm::Value *then_value = render(node->then);
  if ( then_value == 0 ) { return nullptr; }


  builder.CreateBr(merge_block);
  then_block = builder.GetInsertBlock();

  func->getBasicBlockList().push_back(else_block);
  builder.SetInsertPoint(else_block);

  llvm::Value *else_value = render(node->_else);
  if ( else_value == 0 ) { return nullptr; }

  builder.CreateBr(merge_block);
  else_block = builder.GetInsertBlock();

  func->getBasicBlockList().push_back(merge_block);
  builder.SetInsertPoint(merge_block);
  llvm::PHINode *phi_node = builder.CreatePHI(
                                              llvm::Type::getDoubleTy(llvm_context),
                                              2,
                                              "iftmp");

  phi_node->addIncoming(then_value, then_block);
  phi_node->addIncoming(else_value, else_block);

  return phi_node;
}
