#include <string>

#include "variable.h"

VariableNode::VariableNode(const std::string &name)
    : name(name) {}

const std::string
VariableNode::getName() const {
  return name;
}
