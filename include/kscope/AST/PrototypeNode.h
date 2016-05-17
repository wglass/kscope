#pragma once

#include "ASTNode.h"

#include <string>
#include <vector>


static int anon_expr_id = 1;

static std::string
generate_anon_expr_name() {
  std::string name = "anon_expr_";
  name += std::to_string(anon_expr_id);
  anon_expr_id++;
  return name;
}

struct PrototypeNode : public ASTNode {
  std::string name;
  std::vector<std::string> args;
  bool is_anon;

  PrototypeNode(const std::string &name, const std::vector<std::string> &args)
    : ASTNode(ASTNodeKind::Prototype),
      name(name), args(args), is_anon(false) { };

  PrototypeNode(const std::vector<std::string> &args)
    : ASTNode(ASTNodeKind::Prototype),
      name(generate_anon_expr_name()), args(args), is_anon(true) { };
};
