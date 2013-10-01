#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include "ast/if.h"
#include "renderer.h"

using ::llvm::APFloat;
using ::llvm::BasicBlock;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::PHINode;
using ::llvm::Value;
using ::llvm::Type;


Value *
IfNode::codegen(IRRenderer *renderer) {
    Value *cond_value = condition->codegen(renderer);
    if ( cond_value == 0 ) { return 0; }

    cond_value = renderer->builder->CreateFCmpONE(cond_value, ConstantFP::get(renderer->llvm_context(), APFloat(0.0)), "ifcond");

    Function *func = renderer->builder->GetInsertBlock()->getParent();

    BasicBlock *then_block = BasicBlock::Create(renderer->llvm_context(), "then", func);
    BasicBlock *else_block = BasicBlock::Create(renderer->llvm_context(), "else");
    BasicBlock *merge_block = BasicBlock::Create(renderer->llvm_context(), "ifcont");

    renderer->builder->CreateCondBr(cond_value, then_block, else_block);
    renderer->builder->SetInsertPoint(then_block);

    Value *then_value = then->codegen(renderer);
    if ( then_value == 0 ) { return 0; }


    renderer->builder->CreateBr(merge_block);
    then_block = renderer->builder->GetInsertBlock();

    func->getBasicBlockList().push_back(else_block);
    renderer->builder->SetInsertPoint(else_block);

    Value *else_value = _else->codegen(renderer);
    if ( else_value == 0 ) { return 0; }

    renderer->builder->CreateBr(merge_block);
    else_block = renderer->builder->GetInsertBlock();

    func->getBasicBlockList().push_back(merge_block);
    renderer->builder->SetInsertPoint(merge_block);
    PHINode *node = renderer->builder->CreatePHI(
                                                Type::getDoubleTy(renderer->llvm_context()),
                                                2,
                                                "iftmp");

    node->addIncoming(then_value, then_block);
    node->addIncoming(else_value, else_block);

    return node;
}
