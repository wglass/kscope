#pragma once

#include "llvm/IR/Value.h"

#include <string>

#include "node.h"
#include "codegen/renderer.h"


class NumberNode : public ASTNode {
    double val;

public:
    NumberNode(double val);
    virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
