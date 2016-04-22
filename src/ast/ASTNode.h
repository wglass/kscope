#pragma once

#include <utility>
#include <string>


template<class RenderSpec> class Renderer;


class ASTNode {
public:
  virtual ~ASTNode() {};

  template<class RenderSpec> typename RenderSpec::Result* render(Renderer<RenderSpec> *renderer) {
    return renderer->render(this);
  };
};

typedef std::pair<std::string, std::unique_ptr<ASTNode>> NodePair;
