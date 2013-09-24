#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"

#include "ast/binary.h"
#include "ast/variable.h"
#include "context.h"
#include "errors.h"

using ::llvm::Value;
using ::llvm::Type;


Value *
BinaryNode::codegen(Context *context) {
    if ( op == '=' ) {
        VariableNode *lhse = dynamic_cast<VariableNode*>(lhs);
        if ( ! lhse ) {
            return ErrorV("destination of '=' must be a variable");
        }

        Value *val = rhs->codegen(context);
        if ( val == 0 ) { return 0; }

        Value *variable = context->get_named_value(lhse->getName());
        if ( variable == 0 ) {
            return ErrorV("Unknown variable name");
        }

        context->builder()->CreateStore(val, variable);

        return val;
    }

    Value *left = lhs->codegen(context);
    Value *right = rhs->codegen(context);

    if (left == 0 || right == 0 ) { return 0; }

    switch (op) {
    case '+': return context->builder()->CreateFAdd(left, right, "addtmp");
    case '-': return context->builder()->CreateFSub(left, right, "subtmp");
    case '*': return context->builder()->CreateFMul(left, right, "multmp");
    case '<':
        left = context->builder()->CreateFCmpULT(left, right, "cmptmp");
        return context->builder()->CreateUIToFP(left,
                                                Type::getDoubleTy(context->llvm_context()),
                                                "booltmp");
    default: break;
    }

    Function *func = context->module()->getFunction(std::string("binary") + op);
    assert(func && "binary operator not found!");

    Value *operands[2] = { left, right };

    return context->builder()->CreateCall(func, operands, "binop");
}
