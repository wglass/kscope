#pragma once

#include "ASTNode.h"
#include "rendering/Renderer.h"

#include "llvm/IR/Value.h"

#include <string>


struct NumberNode : public ASTNode {
  double val;

  NumberNode(double val) : val(val) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
