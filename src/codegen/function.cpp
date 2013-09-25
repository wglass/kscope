#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/function.h"
#include "context.h"

using ::llvm::BasicBlock;
using ::llvm::Function;
using ::llvm::Value;


Function *
FunctionNode::codegen(Context *context) {
    context->clear_all_named_values();

    Function *func = proto->codegen(context);
    if ( func == 0 ) { return 0; }

    BasicBlock *block = BasicBlock::Create(context->llvm_context(),
                                           "entry",
                                           func);
    context->builder()->SetInsertPoint(block);

    proto->create_argument_allocas(context, func);

    if ( Value *retval = body->codegen(context) ) {
        context->builder()->CreateRet(retval);
        llvm::verifyFunction(*func);

        context->pass_manager()->run(*func);

        return func;
    }

    func->eraseFromParent();

    return 0;
}
