#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include "ast/for.h"
#include "context.h"

using ::llvm::AllocaInst;
using ::llvm::APFloat;
using ::llvm::BasicBlock;
using ::llvm::Constant;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::Value;
using ::llvm::Type;


Value *
ForNode::codegen(Context *context) {
    Function *func = context->builder->GetInsertBlock()->getParent();

    AllocaInst *alloca = context->create_entry_block_alloca(func, var_name);

    Value *start_value = start->codegen(context);
    if ( start_value == 0 ) { return 0; }

    context->builder->CreateStore(start_value, alloca);

    BasicBlock *loop_block = BasicBlock::Create(context->llvm_context(), "loop", func);

    context->builder->CreateBr(loop_block);
    context->builder->SetInsertPoint(loop_block);

    AllocaInst *old_value = context->get_named_value(var_name);
    context->set_named_value(var_name, alloca);

    if ( body->codegen(context) == 0 ) { return 0; }

    Value *step_value;
    if ( step ) {
        step_value = step->codegen(context);
        if ( step_value == 0 ) { return 0; }
    } else {
        step_value = ConstantFP::get(context->llvm_context(), APFloat(1.0));
    }

    Value *end_condition = end->codegen(context);
    if ( end_condition == 0 ) { return 0; }

    Value *current_var = context->builder->CreateLoad(alloca, var_name.c_str());
    Value *next_var = context->builder->CreateFAdd(current_var, step_value, "nextvar");
    context->builder->CreateStore(next_var, alloca);

    end_condition = context->builder->CreateFCmpONE(
        end_condition,
        ConstantFP::get(context->llvm_context(), APFloat(0.0)),
        "loopcond"
    );

    BasicBlock *after_block = BasicBlock::Create(context->llvm_context(), "afterloop", func);

    context->builder->CreateCondBr(end_condition, loop_block, after_block);
    context->builder->SetInsertPoint(after_block);

    if ( old_value ) {
        context->set_named_value(var_name, old_value);
    } else {
        context->clear_named_value(var_name);
    }

    return Constant::getNullValue(Type::getDoubleTy(context->llvm_context()));
}
