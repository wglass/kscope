#include "IRRenderer.h"

#include "IRRenderContext.h"
#include "ORCPipeline/LazyORCPipeline.h"
#include "ast/ASTNode.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"

#include <string>


IRRenderer::IRRenderer()
  : pipeline(std::make_unique<LazyORCPipeline>(this)),
    llvm_context(llvm::getGlobalContext()),
    target_machine(llvm::EngineBuilder().selectTarget()),
    pending_modules(IRRenderer::ModuleSet()) {
}

IRRenderer::IRRenderer(IRRenderer &&other)
  : llvm_context(llvm::getGlobalContext()) {
  pipeline = std::move(other.pipeline);
  render_context = std::move(other.render_context);
  pending_modules = std::move(other.pending_modules);
}

IRRenderer &
IRRenderer::operator =(IRRenderer other) {
    std::swap(pipeline, other.pipeline);
    std::swap(render_context, other.render_context);
    std::swap(pending_modules, other.pending_modules);
    return *this;
}

IRRenderer::~IRRenderer() {
  pipeline.release();
  render_context.release();
}

llvm::TargetMachine &
IRRenderer::get_target_machine() {
  return *target_machine;
}

void
IRRenderer::render(std::shared_ptr<ASTree> tree) {
  render(tree->root.get());

  pending_modules.push_back(render_context->give_up_module());
}

llvm::orc::TargetAddress
IRRenderer::get_function(const std::string &name) {
  flush_modules();

  auto symbol = pipeline->find_unmangled_symbol(name);
  return symbol.getAddress();
}

llvm::Value *
IRRenderer::render(ASTNode *node) {
  return node->render<llvm::Value>(this);
}

IRRenderContext &
IRRenderer::get_render_context() {
  if ( ! render_context || ! render_context->has_module() ) {
    render_context = std::make_unique<IRRenderContext>(llvm_context);
  }

  return *render_context;
}

LazyORCPipeline::ModuleHandle
IRRenderer::flush_modules() {
  LazyORCPipeline::ModuleHandle modules = pipeline->add_modules(std::move(pending_modules));

  pending_modules.clear();

  return modules;
}
