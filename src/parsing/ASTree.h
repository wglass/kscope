#pragma once

#include "ast/ASTNode.h"
#include "ast/FunctionNode.h"
#include "ast/PrototypeNode.h"


class ASTree {
    ASTree(ASTree &&other);
    ASTree &operator =(ASTree other);

public:
  ASTree();
  ~ASTree();

  std::unique_ptr<ASTNode> root;

  void parse(std::istream &input);

  void set_root(std::unique_ptr<FunctionNode> node);
  void set_root(std::unique_ptr<PrototypeNode> node);
};
