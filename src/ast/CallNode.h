#pragma once

#include <string>
#include <vector>

#include "ASTNode.h"


class CallNode : public ASTNode {
public:
  std::string callee;
  std::vector<ASTNode*> args;

  CallNode(const std::string &callee, std::vector<ASTNode*> &args)
    : callee(callee), args(args) {};

  template<class Result> Result* render(Renderer<Result> *renderer) {
    renderer->render(this);
  }
};
