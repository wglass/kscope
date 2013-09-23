#include "ast.h"

CallNode::CallNode(const std::string &callee, std::vector<ASTNode*> &args)
    : callee(callee), args(args) {}
