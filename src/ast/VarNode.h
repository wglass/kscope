#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>
#include <vector>


struct VarNode : public ASTNode {
  std::vector<std::pair<std::string, ASTNode*> > var_names;
  ASTNode *body;

  VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
          ASTNode *body)
    : var_names(var_names), body(body) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
