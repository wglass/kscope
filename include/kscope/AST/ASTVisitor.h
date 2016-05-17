#pragma once

#include "kscope/AST/ASTNodes.h"
#include "kscope/AST/ASTree.h"

#include "llvm/Support/ErrorHandling.h"


template <class Subclass, class Result, typename... ExtraArgTypes>
class ASTVisitor {

public:
  Result *render(ASTNode *node, ExtraArgTypes... extra_args) {
    switch( node->kind ) {
#define AST_NODE(NODE_NAME)                                     \
      case ASTNodeKind::NODE_NAME:                              \
        return static_cast<Subclass*>(this)                     \
          ->render_node(                                        \
                        static_cast<NODE_NAME##Node*>(node),    \
                        std::forward<ExtraArgTypes>(extra_args)...);

#include "kscope/AST/ASTNodes.def"
    }
    llvm_unreachable("Unhanded AST node kind!");
  }

#define AST_NODE(NODE_NAME) \
  virtual Result *render_node(NODE_NAME##Node *node) = 0;

#include "kscope/AST/ASTNodes.def"

  void render_tree(std::shared_ptr<ASTree> tree) {
    render(tree->root.get());
  }
};
