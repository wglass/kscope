#pragma once

#include "ASTNode.h"

#include <string>


struct VariableNode : public ASTNode {
  std::string name;

  VariableNode(const std::string &name)
    : ASTNode(ASTNodeKind::Variable),
      name(name) {};

  const std::string getName() const { return name; };
};
