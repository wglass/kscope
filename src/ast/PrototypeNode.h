#pragma once

#include "ASTNode.h"
#include "rendering/Renderer.h"

#include "llvm/IR/Value.h"

#include <string>
#include <vector>


struct PrototypeNode : public ASTNode {
  std::string name;
  std::vector<std::string> args;

  PrototypeNode(const std::string &name, const std::vector<std::string> &args)
    : name(name), args(args) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
