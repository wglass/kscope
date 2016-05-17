#pragma once

#include "ASTNode.h"

#include <string>


struct ForNode: public ASTNode {
  std::string var_name;
  ASTNode *start, *end, *step, *body;

  ForNode(const std::string &var_name,
          ASTNode *start, ASTNode *end, ASTNode *step,
          ASTNode *body)
    : ASTNode(ASTNodeKind::For),
      var_name(var_name), start(start), end(end), step(step), body(body) {};
};
