#include <string>
#include <vector>

#include "node.h"
#include "call.h"

CallNode::CallNode(const std::string &callee, std::vector<ASTNode*> &args)
    : callee(callee), args(args) {}
