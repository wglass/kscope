#pragma once

#include "ASTNode.h"

#include <string>


struct NumberNode : public ASTNode {
  double val;

  NumberNode(double val) : ASTNode(ASTNodeKind::Number), val(val) {};
};
