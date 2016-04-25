#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>


struct VariableNode : public ASTNode {
  std::string name;

  VariableNode(const std::string &name) : name(name) {};
  const std::string getName() const { return name; };

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
