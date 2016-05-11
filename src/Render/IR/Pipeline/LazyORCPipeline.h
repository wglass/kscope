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


class IRRenderer;
struct FunctionNode;


struct LazyLayerSpec {
  typedef llvm::orc::ObjectLinkingLayer<> ObjectLayer;
  typedef llvm::orc::IRCompileLayer<ObjectLayer> CompileLayer;
  typedef llvm::orc::LazyEmittingLayer<CompileLayer> EmitLayer;

  typedef EmitLayer TopLayer;
};


class LazyORCPipeline : public ORCPipeline<LazyORCPipeline, LazyLayerSpec> {
public:
  typedef LazyLayerSpec::TopLayer::ModuleSetHandleT ModuleHandle;

  LazyORCPipeline(IRRenderer *renderer);

  void process_function_node(FunctionNode *node);

  llvm::orc::JITSymbol find_symbol(const std::string &name);

  ModuleHandle add_modules(ModuleSet &modules);
  void remove_modules(ModuleHandle handle);

private:
  LazyLayerSpec::ObjectLayer object_layer;
  LazyLayerSpec::CompileLayer compile_layer;

  llvm::orc::LocalJITCompileCallbackManager<llvm::orc::OrcX86_64> compile_callbacks;

  std::map<std::string, FunctionNode *> functions;

  llvm::orc::JITSymbol search_functions(const std::string &name);
  void generate_stub(FunctionNode *node);
};
