#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"

#include "ast/binary.h"
#include "ast/variable.h"
#include "renderer.h"
#include "errors.h"

using ::llvm::Value;
using ::llvm::Type;


Value *
BinaryNode::codegen(IRRenderer *renderer) {
    if ( op == '=' ) {
        VariableNode *lhse = dynamic_cast<VariableNode*>(lhs);
        if ( ! lhse ) {
            return ErrorV("destination of '=' must be a variable");
        }

        Value *val = rhs->codegen(renderer);
        if ( val == 0 ) { return 0; }

        Value *variable = renderer->get_named_value(lhse->getName());
        if ( variable == 0 ) {
            return ErrorV("Unknown variable name");
        }

        renderer->builder->CreateStore(val, variable);

        return val;
    }

    Value *left = lhs->codegen(renderer);
    Value *right = rhs->codegen(renderer);

    if (left == 0 || right == 0 ) { return 0; }

    Type *llvm_double_type = Type::getDoubleTy(renderer->llvm_context());

    switch (op) {
    case '+': return renderer->builder->CreateFAdd(left, right, "addtmp");
    case '-': return renderer->builder->CreateFSub(left, right, "subtmp");
    case '*': return renderer->builder->CreateFMul(left, right, "multmp");
    case '<':
        left = renderer->builder->CreateFCmpULT(left, right, "cmptmp");
        return renderer->builder->CreateUIToFP(left,
                                                llvm_double_type,
                                                "booltmp");
    case '>':
        right = renderer->builder->CreateFCmpULT(right, left, "cmptmp");
        return renderer->builder->CreateUIToFP(right,
                                                llvm_double_type,
                                                "booltmp");
    default: break;
    }

    return ErrorV("Unknown binary operator!");
}
