#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include "ast/if.h"
#include "context.h"

using ::llvm::APFloat;
using ::llvm::BasicBlock;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::PHINode;
using ::llvm::Value;
using ::llvm::Type;


Value *
IfNode::codegen(Context *context) {
    Value *cond_value = condition->codegen(context);
    if ( cond_value == 0 ) { return 0; }

    cond_value = context->builder()->CreateFCmpONE(cond_value, ConstantFP::get(context->llvm_context(), APFloat(0.0)), "ifcond");

    Function *func = context->builder()->GetInsertBlock()->getParent();

    BasicBlock *then_block = BasicBlock::Create(context->llvm_context(), "then", func);
    BasicBlock *else_block = BasicBlock::Create(context->llvm_context(), "else");
    BasicBlock *merge_block = BasicBlock::Create(context->llvm_context(), "ifcont");

    context->builder()->CreateCondBr(cond_value, then_block, else_block);
    context->builder()->SetInsertPoint(then_block);

    Value *then_value = then->codegen(context);
    if ( then_value == 0 ) { return 0; }


    context->builder()->CreateBr(merge_block);
    then_block = context->builder()->GetInsertBlock();

    func->getBasicBlockList().push_back(else_block);
    context->builder()->SetInsertPoint(else_block);

    Value *else_value = _else->codegen(context);
    if ( else_value == 0 ) { return 0; }

    context->builder()->CreateBr(merge_block);
    else_block = context->builder()->GetInsertBlock();

    func->getBasicBlockList().push_back(merge_block);
    context->builder()->SetInsertPoint(merge_block);
    PHINode *node = context->builder()->CreatePHI(Type::getDoubleTy(context->llvm_context()),
                                                  2,
                                                  "iftmp");

    node->addIncoming(then_value, then_block);
    node->addIncoming(else_value, else_block);

    return node;
}
