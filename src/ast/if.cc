#include "node.h"
#include "if.h"

IfNode::IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
    : condition(cond), then(then), _else(_else) {}
