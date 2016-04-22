#pragma once

#include <string>

#include "ASTNode.h"


class VariableNode : public ASTNode {
public:
  std::string name;

  VariableNode(const std::string &name) : name(name) {};
  const std::string getName() const { return name; };

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
