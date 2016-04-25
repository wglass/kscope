#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>


struct NumberNode : public ASTNode {
  double val;

  NumberNode(double val) : val(val) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
