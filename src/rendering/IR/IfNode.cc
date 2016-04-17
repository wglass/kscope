#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include "ast/IfNode.h"

#include "IRRenderer.h"


using ::llvm::APFloat;
using ::llvm::BasicBlock;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::PHINode;
using ::llvm::Value;
using ::llvm::Type;


Value *
IRRenderer::render(IfNode *node) {
  Value *cond_value = render(node->condition);
    if ( cond_value == 0 ) { return 0; }

    cond_value = builder->CreateFCmpONE(cond_value, ConstantFP::get(llvm_context(), APFloat(0.0)), "ifcond");

    Function *func = builder->GetInsertBlock()->getParent();

    BasicBlock *then_block = BasicBlock::Create(llvm_context(), "then", func);
    BasicBlock *else_block = BasicBlock::Create(llvm_context(), "else");
    BasicBlock *merge_block = BasicBlock::Create(llvm_context(), "ifcont");

    builder->CreateCondBr(cond_value, then_block, else_block);
    builder->SetInsertPoint(then_block);

    Value *then_value = render(node->then);
    if ( then_value == 0 ) { return 0; }


    builder->CreateBr(merge_block);
    then_block = builder->GetInsertBlock();

    func->getBasicBlockList().push_back(else_block);
    builder->SetInsertPoint(else_block);

    Value *else_value = render(node->_else);
    if ( else_value == 0 ) { return 0; }

    builder->CreateBr(merge_block);
    else_block = builder->GetInsertBlock();

    func->getBasicBlockList().push_back(merge_block);
    builder->SetInsertPoint(merge_block);
    PHINode *phi_node = builder->CreatePHI(Type::getDoubleTy(llvm_context()),
                                           2,
                                           "iftmp");

    phi_node->addIncoming(then_value, then_block);
    phi_node->addIncoming(else_value, else_block);

    return phi_node;
}
