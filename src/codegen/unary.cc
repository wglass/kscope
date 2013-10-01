#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "ast/unary.h"
#include "renderer.h"
#include "errors.h"

using ::llvm::Value;
using ::llvm::Function;


Value *
UnaryNode::codegen(IRRenderer *renderer) {
    Value *operand_value = operand->codegen(renderer);
    if ( operand_value == 0 ) { return 0; }

    Function *func = renderer->module->getFunction(std::string("unary") + opcode);
    if ( func == 0 ) {
        return ErrorV("Unknown unary operator");
    }

    return renderer->builder->CreateCall(func, operand_value, "unop");
}
