#include "IRRenderer.h"

#include "IRContext.h"

#include "parsing/ASTree.h"
#include "rendering/IR/Pipeline/Pipeline.h"
#include "rendering/IR/Pipeline/LazyORCPipeline.h"
#include "rendering/IR/Pipeline/SimpleORCPipeline.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"

#include <string>


IRRenderer::IRRenderer(PipelineChoice pipeline_choice)
  : pending_modules(std::make_unique<ModuleSet>()),
    proto_map(std::make_unique<ProtoMap>()),
    target_machine(llvm::EngineBuilder().selectTarget()),
    data_layout(target_machine->createDataLayout()) {
  switch (pipeline_choice) {
  case PipelineChoice::Lazy:
    pipeline = std::make_unique<LazyORCPipeline>(this);
    break;
  case PipelineChoice::Simple:
    pipeline = std::make_unique<SimpleORCPipeline>(this);
    break;
  }
}

IRRenderer::IRRenderer(IRRenderer &&other)
  : data_layout(other.data_layout) {
  render_context = std::move(other.render_context);
  pipeline = std::move(other.pipeline);
  pending_modules = std::move(other.pending_modules);
}

IRRenderer &
IRRenderer::operator =(IRRenderer other) {
    std::swap(render_context, other.render_context);
    std::swap(pipeline, other.pipeline);
    std::swap(pending_modules, other.pending_modules);
    std::swap(proto_map, other.proto_map);
    return *this;
}

IRRenderer::~IRRenderer() {
  pipeline.release();
  render_context.release();
  pending_modules.release();
  proto_map.release();
}

llvm::TargetMachine &
IRRenderer::get_target_machine() {
  return *target_machine;
}

const llvm::DataLayout
IRRenderer::get_data_layout() {
  return data_layout;
}

void
IRRenderer::render_tree(std::shared_ptr<ASTree> tree) {
  render(tree->root.get());
}

llvm::orc::TargetAddress
IRRenderer::get_symbol(const std::string &name) {
  flush_modules();

  auto symbol = pipeline->find_unmangled_symbol(name);
  return symbol.getAddress();
}

PrototypeNode *
IRRenderer::get_prototype(const std::string &name) {
  auto search = proto_map->find(name);
  if ( search == proto_map->end() ) {
    return nullptr;
  }

  return search->second;
}

IRContext &
IRRenderer::get_render_context() {
  if ( ! render_context || ! render_context->has_module() ) {
    render_context = std::make_unique<IRContext>(data_layout);
  }

  return *render_context;
}

void
IRRenderer::flush_modules() {
  if ( render_context && render_context->has_module() ) {
    pending_modules->push_back(render_context->give_up_module());
  }

  if ( pending_modules->empty() ) {
    fprintf(stderr, "No modules to flush!\n");
    return;
  }

  pipeline->flush_modules(*pending_modules);
  pending_modules->clear();
}
