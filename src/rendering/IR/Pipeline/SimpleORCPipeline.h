#pragma once

#include "ORCPipeline.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"


class IRRenderer;
struct FunctionNode;

struct SimpleLayerSpec {
  typedef llvm::orc::ObjectLinkingLayer<> ObjectLayer;
  typedef llvm::orc::IRCompileLayer<ObjectLayer> CompileLayer;

  typedef CompileLayer TopLayer;
};


class SimpleORCPipeline : public ORCPipeline<SimpleORCPipeline, SimpleLayerSpec> {
public:
  typedef SimpleLayerSpec::TopLayer::ModuleSetHandleT ModuleHandle;

  SimpleORCPipeline(IRRenderer *renderer);

  void process_function_node(FunctionNode *node);

  ModuleHandle add_modules(ModuleSet &modules);
  void remove_modules(ModuleHandle handle);

private:
  SimpleLayerSpec::ObjectLayer object_layer;
};
