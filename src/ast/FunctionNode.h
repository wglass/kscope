#pragma once

#include "ASTNode.h"
#include "PrototypeNode.h"


class FunctionNode : public ASTNode {
public:
  PrototypeNode *proto;
  ASTNode *body;

  FunctionNode(PrototypeNode *proto, ASTNode *body)
    : proto(proto), body(body) {};
};
