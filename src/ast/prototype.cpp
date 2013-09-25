#include <string>
#include <vector>

#include "prototype.h"


PrototypeNode::PrototypeNode(const std::string &name,
                             const std::vector<std::string> &args)
    : name(name), args(args) {}
