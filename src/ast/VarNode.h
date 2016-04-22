#pragma once

#include <string>
#include <vector>

#include "ASTNode.h"


class VarNode : public ASTNode {
public:
  std::vector<std::pair<std::string, ASTNode*> > var_names;
  ASTNode *body;

  VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
          ASTNode *body)
    : var_names(var_names), body(body) {};

  template<class RenderSpec> typename RenderSpec::Result* render(Renderer<RenderSpec> *renderer) {
    return renderer->render(this);
  };
};
