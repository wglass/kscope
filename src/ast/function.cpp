#include "ast.h"


FunctionNode::FunctionNode(PrototypeNode *proto, ASTNode *body)
    : proto(proto), body(body) {}
