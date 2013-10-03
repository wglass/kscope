#include <string>

#include "node.h"
#include "for.h"


ForNode::ForNode(const std::string &var_name,
                 ASTNode *start, ASTNode *end,
                 ASTNode *step,
                 ASTNode *body)
    : var_name(var_name), start(start), end(end), step(step), body(body) {}
