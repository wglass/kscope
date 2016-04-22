#pragma once

#include <string>
#include <vector>

#include "ASTNode.h"


class VarNode : public ASTNode {
public:
  std::unique_ptr<std::vector<NodePair> > var_names;
  std::unique_ptr<ASTNode> body;

  VarNode(std::unique_ptr<std::vector<NodePair> > var_names,
          std::unique_ptr<ASTNode> body)
    : var_names(std::move(var_names)), body(std::move(body)) {};
};
