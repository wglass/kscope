#include "IRRenderer.h"

#include "IRContext.h"
#include "ast/ASTNode.h"

#include "rendering/IR/ORCPipeline/LazyORCPipeline.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"

#include <string>


template <class Pipeline>
IRRenderer<Pipeline>::IRRenderer()
  : pipeline(std::make_unique<Pipeline>(this)),
    pending_modules(IRRenderer<Pipeline>::ModuleSet()) {
}

template <class Pipeline>
IRRenderer<Pipeline>::IRRenderer(IRRenderer &&other) {
  render_context = std::move(other.render_context);
  pipeline = std::move(other.pipeline);
  pending_modules = std::move(other.pending_modules);
}

template <class Pipeline>
IRRenderer<Pipeline> &
IRRenderer<Pipeline>::operator =(IRRenderer other) {
    std::swap(render_context, other.render_context);
    std::swap(pipeline, other.pipeline);
    std::swap(pending_modules, other.pending_modules);
    return *this;
}

template <class Pipeline>
IRRenderer<Pipeline>::~IRRenderer() {
  pipeline.release();
  render_context.release();
}

template <class Pipeline>
void
IRRenderer<Pipeline>::render_tree(std::shared_ptr<ASTree> tree) {
  auto &context = get_render_context();

  render(tree->root.get());
  pending_modules.push_back(context.give_up_module());
}

template <class Pipeline>
llvm::orc::TargetAddress
IRRenderer<Pipeline>::get_function(const std::string &name) {
  flush_modules();

  auto symbol = pipeline->find_unmangled_symbol(name);
  return symbol.getAddress();
}

template <class Pipeline>
IRContext &
IRRenderer<Pipeline>::get_render_context() {
  if ( ! render_context || ! render_context->has_module() ) {
    render_context = std::make_unique<IRContext>();
  }

  return *render_context;
}

template <class Pipeline>
typename Pipeline::ModuleHandle
IRRenderer<Pipeline>::flush_modules() {
  fprintf(stderr, "Flushing %lu llvm modules\n", pending_modules.size());
  typename Pipeline::ModuleHandle modules = pipeline->add_modules(std::move(pending_modules));

  pending_modules.clear();

  return modules;
}


class LazyIRRenderer : IRRenderer<LazyORCPipeline> {};
