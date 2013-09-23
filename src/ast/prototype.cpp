#include <string>
#include <vector>

#include "ast.h"


PrototypeNode::PrototypeNode(const std::string &name,
                             const std::vector<std::string> &args,
                             bool is_operator,
                             unsigned precedence)
    : name(name), args(args), is_operator(is_operator), precedence(precedence) {}


bool
PrototypeNode::isUnaryOp() const {
    return is_operator && args.size() == 1;
}

bool
PrototypeNode::isBinaryOp() const {
    return is_operator && args.size() == 2;
};

char
PrototypeNode::getOperatorName() const {
    assert(isUnaryOp() || isBinaryOp());
    return name[name.size() - 1];
};

unsigned
PrototypeNode::getBinaryPrecedence() const {
    return precedence;
};
