#pragma once

#include "ASTNode.h"
#include "PrototypeNode.h"


class FunctionNode : public ASTNode {
public:
  std::unique_ptr<PrototypeNode> proto;
  std::unique_ptr<ASTNode> body;

  FunctionNode(std::unique_ptr<PrototypeNode> proto,
               std::unique_ptr<ASTNode> body)
    : proto(std::move(proto)), body(std::move(body)) {};

};
