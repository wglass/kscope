#include "node.h"
#include "binary.h"

BinaryNode::BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
    : op(op), lhs(lhs), rhs(rhs) {}
