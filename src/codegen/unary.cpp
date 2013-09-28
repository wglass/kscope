#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/unary.h"
#include "context.h"
#include "errors.h"

using ::llvm::Value;
using ::llvm::Function;


Value *
UnaryNode::codegen(Context *context) {
    Value *operand_value = operand->codegen(context);
    if ( operand_value == 0 ) { return 0; }

    Function *func = context->module->getFunction(std::string("unary") + opcode);
    if ( func == 0 ) {
        return ErrorV("Unknown unary operator");
    }

    return context->builder->CreateCall(func, operand_value, "unop");
}
