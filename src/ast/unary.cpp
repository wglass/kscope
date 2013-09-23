#include "node.h"
#include "unary.h"

UnaryNode::UnaryNode(char opcode, ASTNode *operand)
    : opcode(opcode), operand(operand) {}
