#pragma once

#include "ASTNode.h"

#include <string>
#include <vector>


struct VarNode : public ASTNode {
  std::vector<std::pair<std::string, ASTNode*> > var_names;
  ASTNode *body;

  VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
          ASTNode *body)
    : ASTNode(ASTNodeKind::Var),
      var_names(var_names), body(body) {};
};
