#pragma once

#include "ASTNode.h"

#include "PrototypeNode.h"


struct FunctionNode : public ASTNode {
  PrototypeNode *proto;
  ASTNode *body;

  FunctionNode(PrototypeNode *proto, ASTNode *body)
    : ASTNode(ASTNodeKind::Function),
      proto(proto), body(body) {};
};
