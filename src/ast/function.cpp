#include "node.h"
#include "prototype.h"
#include "function.h"

FunctionNode::FunctionNode(PrototypeNode *proto, ASTNode *body)
    : proto(proto), body(body) {}
