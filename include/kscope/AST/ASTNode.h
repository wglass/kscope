#pragma once


enum class ASTNodeKind {
#define AST_NODE(NODE_NAME) NODE_NAME,
#include "kscope/AST/ASTNodes.def"
};

struct ASTNode {
  ASTNodeKind kind;

  ASTNode(ASTNodeKind kind) : kind(kind) {};
};
