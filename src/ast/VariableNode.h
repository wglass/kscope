#pragma once

#include "ASTNode.h"
#include "rendering/Renderer.h"

#include "llvm/IR/Value.h"

#include <string>


struct VariableNode : public ASTNode {
  std::string name;

  VariableNode(const std::string &name) : name(name) {};
  const std::string getName() const { return name; };

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
