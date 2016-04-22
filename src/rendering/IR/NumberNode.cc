#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "ast/NumberNode.h"

#include "IRRenderer.h"


llvm::Value *
IRRenderer::render(NumberNode *node) {
    return llvm::ConstantFP::get(llvm_context, llvm::APFloat(node->val));
}
