#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include <cstddef>
#include <string>
#include <vector>

#include "ast/ASTNode.h"
#include "ast/VarNode.h"

#include "IRRenderer.h"


using ::llvm::AllocaInst;
using ::llvm::APFloat;
using ::llvm::ConstantFP;


llvm::Value *
IRRenderer::render(VarNode *node) {
    std::vector<AllocaInst *> old_bindings;

    llvm::Function *func = builder->GetInsertBlock()->getParent();

    for ( auto &var_pair : node->var_names) {
        const std::string &var_name = var_pair.first;
        ASTNode *init = var_pair.second;

        llvm::Value *init_val;
        if ( init ) {
          init_val = render(init);
            if ( init_val == 0) { return 0; }
        } else {
            init_val = ConstantFP::get(llvm_context(), APFloat(0.0));
        }

        AllocaInst *alloca = create_entry_block_alloca(func, var_name);
        builder->CreateStore(init_val, alloca);

        old_bindings.push_back(get_named_value(var_name));
        set_named_value(var_name, alloca);
    }

    llvm::Value *body_val = render(node->body);
    if ( body_val == 0 ) { return 0; }

    std::size_t index = 0;
    for ( auto &var_pair : node->var_names) {
        set_named_value(var_pair.first, old_bindings[index]);
        ++index;
    }

    return body_val;
}
