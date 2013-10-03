#pragma once

#include "ast.h"


class STree {
    STree(STree &&other);
    STree &operator =(STree other);

public:
    STree();
    ~STree();

    unique_ptr<ASTNode> root;

    void parse(std::istream &input);

    void set_root(FunctionNode *node);
    void set_root(PrototypeNode *node);
};
