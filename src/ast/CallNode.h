#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>
#include <vector>


struct CallNode : public ASTNode {
  std::string callee;
  std::vector<ASTNode*> args;

  CallNode(const std::string &callee, std::vector<ASTNode*> &args)
    : callee(callee), args(args) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
