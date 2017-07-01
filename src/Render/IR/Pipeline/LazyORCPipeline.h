#pragma once

#include "ORCPipeline.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/OrcABISupport.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
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

  ModuleHandle add_modules(ModuleSet &modules);
  void remove_modules(ModuleHandle handle);

private:
  LazyLayerSpec::ObjectLayer object_layer;
  LazyLayerSpec::CompileLayer compile_layer;

  std::unique_ptr<llvm::TargetMachine> target_machine;
  const llvm::DataLayout data_layout;

  std::unique_ptr<llvm::orc::JITCompileCallbackManager> callback_manager;
  std::unique_ptr<llvm::orc::IndirectStubsManager> stub_manager;

  std::map<std::string, FunctionNode *> functions;
};
