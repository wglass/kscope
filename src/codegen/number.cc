#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "ast/number.h"
#include "renderer.h"


llvm::Value *
NumberNode::codegen(IRRenderer *renderer) {
    return llvm::ConstantFP::get(renderer->llvm_context(), llvm::APFloat(val));
}
