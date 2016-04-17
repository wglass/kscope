#pragma once


template<class Result> class Renderer;


class ASTNode {
public:
  virtual ~ASTNode() {};

  template<class Result> Result* render(Renderer<Result> *renderer) {
    return renderer->render(this);
  };
};
