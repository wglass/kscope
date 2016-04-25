#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>


struct ForNode: public ASTNode {
  std::string var_name;
  ASTNode *start, *end, *step, *body;

  ForNode(const std::string &var_name,
          ASTNode *start, ASTNode *end, ASTNode *step,
          ASTNode *body)
    : var_name(var_name), start(start), end(end), step(step), body(body) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
