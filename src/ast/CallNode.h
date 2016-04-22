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

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
