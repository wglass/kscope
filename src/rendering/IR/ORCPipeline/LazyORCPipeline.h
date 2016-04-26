#pragma once

#include "ORCPipeline.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/OrcArchitectureSupport.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/IR/Module.h"


template <class Pipeline> class IRRenderer;
struct FunctionNode;


struct LazyLayerSpec {
  typedef llvm::orc::ObjectLinkingLayer<> ObjectLayer;
  typedef llvm::orc::IRCompileLayer<ObjectLayer> CompileLayer;
  typedef llvm::orc::LazyEmittingLayer<CompileLayer> EmitLayer;

  typedef EmitLayer TopLayer;
  typedef EmitLayer::ModuleSetHandleT ModuleHandle;
};


class LazyORCPipeline : public ORCPipeline<LazyLayerSpec> {

public:
  LazyORCPipeline(IRRenderer<ORCPipeline<LazyLayerSpec>> *renderer);

  void add_function(FunctionNode *node);

  LazyLayerSpec::ModuleHandle add_modules(ModuleSet modules);
  void remove_modules(LazyLayerSpec::ModuleHandle handle);

private:
  LazyLayerSpec::ObjectLayer object_layer;
  LazyLayerSpec::CompileLayer compile_layer;

  llvm::orc::LocalJITCompileCallbackManager<llvm::orc::OrcX86_64> compile_callbacks;

  std::map<std::string, FunctionNode *> functions;

  llvm::RuntimeDyld::SymbolInfo search_functions(const std::string &name);
  LazyLayerSpec::ModuleHandle generate_stub(FunctionNode *node);
};
