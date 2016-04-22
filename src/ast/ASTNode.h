#pragma once

#include <utility>
#include <string>


template<class Result, typename Function> class Renderer;


class ASTNode {
public:
  virtual ~ASTNode() {};

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};

typedef std::pair<std::string, std::unique_ptr<ASTNode>> NodePair;
