#pragma once

#include <string>

#include "ASTNode.h"


class NumberNode : public ASTNode {
public:
  double val;

  NumberNode(double val) : val(val) {};
};
