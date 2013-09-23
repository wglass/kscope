#include <string>
#include <vector>

#include "node.h"
#include "var.h"


VarNode::VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
                 ASTNode *body)
    : var_names(var_names), body(body) {}
