#include "IRRenderer.h"

#include "ast/CallNode.h"
#include "errors/Error.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include <vector>


llvm::Value *
IRRenderer::render(CallNode *node) {
  auto &context = get_render_context();
  auto &module = context.get_module();
  auto &builder = context.get_builder();

  llvm::Function *callee_func = module.getFunction(node->callee);
  if ( callee_func == 0 ) {
    return Error<llvm::Value>::handle("Unknown function referenced");
  }

  if ( callee_func->arg_size() != node->args.size() ) {
    return Error<llvm::Value>::handle("Incorrect number of arguments passed");
  }

  std::vector<llvm::Value*> arg_values;
  for ( auto &arg : node->args ) {
    arg_values.push_back(render(arg));
    if ( arg_values.back() == 0 ) { return nullptr; }
  }

  return builder.CreateCall(callee_func, arg_values, "calltmp");
}
