#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include "ast/ForNode.h"

#include "IRRenderer.h"


using ::llvm::AllocaInst;
using ::llvm::APFloat;
using ::llvm::BasicBlock;
using ::llvm::Constant;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::Value;
using ::llvm::Type;


llvm::Value *
IRRenderer::render(ForNode *node) {
    Function *func = builder->GetInsertBlock()->getParent();

    AllocaInst *alloca = create_entry_block_alloca(func, node->var_name);

    Value *start_value = render(node->start);
    if ( start_value == 0 ) { return 0; }

    builder->CreateStore(start_value, alloca);

    BasicBlock *loop_block = BasicBlock::Create(llvm_context(), "loop", func);

    builder->CreateBr(loop_block);
    builder->SetInsertPoint(loop_block);

    AllocaInst *old_value = get_named_value(node->var_name);
    set_named_value(node->var_name, alloca);

    if ( render(node->body) == 0 ) { return 0; }

    Value *step_value;
    if ( node->step ) {
      step_value = render(node->step);
        if ( step_value == 0 ) { return 0; }
    } else {
        step_value = ConstantFP::get(llvm_context(), APFloat(1.0));
    }

    Value *end_condition = render(node->end);
    if ( end_condition == 0 ) { return 0; }

    Value *current_var = builder->CreateLoad(alloca, node->var_name.c_str());
    Value *next_var = builder->CreateFAdd(current_var, step_value, "nextvar");
    builder->CreateStore(next_var, alloca);

    end_condition = builder->CreateFCmpONE(
        end_condition,
        ConstantFP::get(llvm_context(), APFloat(0.0)),
        "loopcond"
    );

    BasicBlock *after_block = BasicBlock::Create(llvm_context(), "afterloop", func);

    builder->CreateCondBr(end_condition, loop_block, after_block);
    builder->SetInsertPoint(after_block);

    if ( old_value ) {
        set_named_value(node->var_name, old_value);
    } else {
        clear_named_value(node->var_name);
    }

    return Constant::getNullValue(Type::getDoubleTy(llvm_context()));
}
