#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "ast/node.h"
#include "ast/var.h"
#include "context.h"

#include <string>
#include <vector>

using ::llvm::AllocaInst;
using ::llvm::APFloat;
using ::llvm::ConstantFP;
using ::llvm::Function;
using ::llvm::Value;


Value *
VarNode::codegen(Context *context) {
    std::vector<AllocaInst *> old_bindings;

    Function *func = context->builder()->GetInsertBlock()->getParent();

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        const std::string &var_name = var_names[i].first;
        ASTNode *init = var_names[i].second;

        Value *init_val;
        if ( init ) {
            init_val = init->codegen(context);
            if ( init_val == 0) { return 0; }
        } else {
            init_val = ConstantFP::get(context->llvm_context(), APFloat(0.0));
        }

        AllocaInst *alloca = context->create_entry_block_alloca(func, var_name);
        context->builder()->CreateStore(init_val, alloca);

        old_bindings.push_back(context->get_named_value(var_name));
        context->set_named_value(var_name, alloca);
    }

    Value *body_val = body->codegen(context);
    if ( body_val == 0 ) { return 0; }

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        context->set_named_value(var_names[i].first, old_bindings[i]);
    }

    return body_val;
}
