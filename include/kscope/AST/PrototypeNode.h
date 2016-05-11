#pragma once

#include "ASTNode.h"

#include "kscope/Render/Renderer.h"

#include "llvm/IR/Value.h"

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
    : name(name), args(args), is_anon(false) { };

  PrototypeNode(const std::vector<std::string> &args)
    : name(generate_anon_expr_name()), args(args), is_anon(true) { };

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
