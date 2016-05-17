#pragma once

#include "kscope/AST/ASTNodes.h"
#include "llvm/Support/ErrorHandling.h"


template <class Subclass, class Result, typename... ExtraArgTypes>
class ASTVisitor {

public:
  Result *visit(ASTNode *node, ExtraArgTypes... extra_args) {
    switch( node->kind ) {
#define AST_NODE(NODE_NAME)                                             \
      case ASTNodeKind::NODE_NAME:                                      \
        return static_cast<Subclass*>(this)                             \
          ->visit_node(                                                 \
                       static_cast<NODE_NAME##Node*>(node),             \
                       std::forward<ExtraArgTypes>(extra_args)...);

#include "kscope/AST/ASTNodes.def"
    }
    llvm_unreachable("Unhanded AST node kind!");
  }

#define AST_NODE(NODE_NAME) \
  virtual Result *visit_node(NODE_NAME##Node *node) = 0;

#include "kscope/AST/ASTNodes.def"
};
