#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "ast/number.h"
#include "context.h"


llvm::Value *
NumberNode::codegen(Context *context) {
    return llvm::ConstantFP::get(context->llvm_context(), llvm::APFloat(val));
}
