#include "IRRenderer.h"

#include "IRContext.h"
#include "ast/ASTNode.h"

#include "rendering/IR/Pipeline/LazyORCPipeline.h"
#include "rendering/IR/Pipeline/SimpleORCPipeline.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"

#include <string>


IRRenderer::IRRenderer(PipelineChoice pipeline_choice)
  : pending_modules(std::make_unique<ModuleSet>()) {
  switch (pipeline_choice) {
  case PipelineChoice::Lazy:
    pipeline = std::make_unique<LazyORCPipeline>(this);
    break;
  case PipelineChoice::Simple:
    pipeline = std::make_unique<SimpleORCPipeline>(this);
    break;
  }
}

IRRenderer::IRRenderer(IRRenderer &&other) {
  render_context = std::move(other.render_context);
  pipeline = std::move(other.pipeline);
  pending_modules = std::move(other.pending_modules);
}

IRRenderer &
IRRenderer::operator =(IRRenderer other) {
    std::swap(render_context, other.render_context);
    std::swap(pipeline, other.pipeline);
    std::swap(pending_modules, other.pending_modules);
    return *this;
}

IRRenderer::~IRRenderer() {
  pipeline.release();
  render_context.release();
  pending_modules.release();
}

void
IRRenderer::render_tree(std::shared_ptr<ASTree> tree) {
  auto &context = get_render_context();

  render(tree->root.get());
  pending_modules->push_back(context.give_up_module());
}

llvm::orc::TargetAddress
IRRenderer::get_function(const std::string &name) {
  flush_modules();

  auto symbol = pipeline->find_unmangled_symbol(name);
  return symbol.getAddress();
}

IRContext &
IRRenderer::get_render_context() {
  if ( ! render_context || ! render_context->has_module() ) {
    render_context = std::make_unique<IRContext>();
  }

  return *render_context;
}

void
IRRenderer::flush_modules() {
  fprintf(stderr, "Flushing %lu llvm modules\n", pending_modules->size());
  pipeline->flush_modules(*pending_modules);
  pending_modules->clear();
}
