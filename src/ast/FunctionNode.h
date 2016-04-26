#pragma once

#include "ASTNode.h"

#include "PrototypeNode.h"
#include "rendering/Renderer.h"

#include "llvm/IR/Value.h"


struct FunctionNode : public ASTNode {
  PrototypeNode *proto;
  ASTNode *body;

  FunctionNode(PrototypeNode *proto, ASTNode *body)
    : proto(proto), body(body) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
