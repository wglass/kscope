#include "ast.h"


UnaryNode::UnaryNode(char opcode, ASTNode *operand)
    : opcode(opcode), operand(operand) {}
