#include "llvm/IR/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/FunctionNode.h"

#include "IRRenderer.h"


llvm::Function *
IRRenderer::render(FunctionNode *node) {
    clear_all_named_values();

    llvm::Function *func = render(node->proto);
    if ( func == 0 ) { return 0; }

    llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm_context(),
                                                       "entry",
                                                       func);
    builder->SetInsertPoint(block);

    create_argument_allocas(func, node->proto->args);

    if ( llvm::Value *retval = render(node->body) ) {
        builder->CreateRet(retval);
        llvm::verifyFunction(*func);

        pass_manager->run(*func);

        return func;
    }

    func->eraseFromParent();

    return nullptr;
}
