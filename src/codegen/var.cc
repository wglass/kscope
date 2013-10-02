#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "ast/node.h"
#include "ast/var.h"
#include "renderer.h"

#include <cstddef>
#include <string>
#include <vector>

using ::llvm::AllocaInst;
using ::llvm::APFloat;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::Value;


Value *
VarNode::codegen(IRRenderer *renderer) {
    std::vector<AllocaInst *> old_bindings;

    Function *func = renderer->builder->GetInsertBlock()->getParent();

    for ( auto &var_pair : var_names) {
        const std::string &var_name = var_pair.first;
        ASTNode *init = var_pair.second;

        Value *init_val;
        if ( init ) {
            init_val = init->codegen(renderer);
            if ( init_val == 0) { return 0; }
        } else {
            init_val = ConstantFP::get(renderer->llvm_context(), APFloat(0.0));
        }

        AllocaInst *alloca = renderer->create_entry_block_alloca(func, var_name);
        renderer->builder->CreateStore(init_val, alloca);

        old_bindings.push_back(renderer->get_named_value(var_name));
        renderer->set_named_value(var_name, alloca);
    }

    Value *body_val = body->codegen(renderer);
    if ( body_val == 0 ) { return 0; }

    std::size_t index = 0;
    for ( auto &var_pair : var_names) {
        renderer->set_named_value(var_pair.first, old_bindings[index]);
        ++index;
    }

    return body_val;
}
