#include <string>

#include "ast.h"


NumberNode::NumberNode(double val) : val(val) {}

VariableNode::VariableNode(const std::string &name) :
    name(name) {}

const std::string
VariableNode::getName() const {
    return name;
}

BinaryNode::BinaryNode(char op, ASTNode *lhs, ASTNode *rhs) :
    op(op), lhs(lhs), rhs(rhs) {}

UnaryNode::UnaryNode(char opcode, ASTNode *operand) :
    opcode(opcode), operand(operand) {}

CallNode::CallNode(const std::string &callee, std::vector<ASTNode*> &args) :
    callee(callee), args(args) {}

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


FunctionNode::FunctionNode(PrototypeNode *proto, ASTNode *body) :
    proto(proto), body(body) {}


VarNode::VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
                 ASTNode *body)
    : var_names(var_names), body(body) {}


IfNode::IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else) :
    condition(cond), then(then), _else(_else) {}


ForNode::ForNode(const std::string &var_name,
                 ASTNode *start, ASTNode *end, ASTNode *step,
                 ASTNode *body) :
    var_name(var_name), start(start), end(end), step(step), body(body) {}
