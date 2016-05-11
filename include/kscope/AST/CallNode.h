#pragma once

#include "ASTNode.h"

#include "kscope/Render/Renderer.h"

#include "llvm/IR/Value.h"

#include <string>
#include <vector>


struct CallNode : public ASTNode {
  std::string callee;
  std::vector<ASTNode*> args;

  CallNode(const std::string &callee, std::vector<ASTNode*> &args)
    : callee(callee), args(args) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
