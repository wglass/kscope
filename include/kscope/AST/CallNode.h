#pragma once

#include "ASTNode.h"

#include <string>
#include <vector>


struct CallNode : public ASTNode {
  std::string callee;
  std::vector<ASTNode*> args;

   CallNode(const std::string &callee, std::vector<ASTNode*> &args)
     : ASTNode(ASTNodeKind::Call),
       callee(callee), args(args) {};
};
