#pragma once

#include "ASTNode.h"

#include "PrototypeNode.h"
#include "rendering/IR/IRRenderer.h"


struct FunctionNode : public ASTNode {
  PrototypeNode *proto;
  ASTNode *body;

  FunctionNode(PrototypeNode *proto, ASTNode *body)
    : proto(proto), body(body) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
