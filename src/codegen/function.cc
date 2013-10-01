#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/function.h"
#include "renderer.h"

using ::llvm::BasicBlock;
using ::llvm::Function;
using ::llvm::Value;


Function *
FunctionNode::codegen(IRRenderer *renderer) {
    renderer->clear_all_named_values();

    Function *func = proto->codegen(renderer);
    if ( func == 0 ) { return 0; }

    BasicBlock *block = BasicBlock::Create(renderer->llvm_context(),
                                           "entry",
                                           func);
    renderer->builder->SetInsertPoint(block);

    proto->create_argument_allocas(renderer, func);

    if ( Value *retval = body->codegen(renderer) ) {
        renderer->builder->CreateRet(retval);
        llvm::verifyFunction(*func);

        renderer->pass_manager->run(*func);

        return func;
    }

    func->eraseFromParent();

    return 0;
}
