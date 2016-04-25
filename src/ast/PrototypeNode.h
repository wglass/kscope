#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>
#include <vector>


struct PrototypeNode : public ASTNode {
  std::string name;
  std::vector<std::string> args;

  PrototypeNode(const std::string &name, const std::vector<std::string> &args)
    : name(name), args(args) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
