#pragma once

#include "ORCPipeline.h"

#include "ast/FunctionNode.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"


struct SimpleLayerSpec {
  typedef llvm::orc::ObjectLinkingLayer<> ObjectLayer;
  typedef llvm::orc::IRCompileLayer<ObjectLayer> CompileLayer;

  typedef CompileLayer TopLayer;
};


class SimpleORCPipeline : ORCPipeline<SimpleORCPipeline, SimpleLayerSpec> {
public:
  typedef SimpleLayerSpec::TopLayer::ModuleSetHandleT ModuleHandle;

  SimpleORCPipeline(IRRenderer<SimpleORCPipeline> *renderer);

  void add_function(FunctionNode *node);

  ModuleHandle add_modules(ModuleSet module);
  void remove_modules(ModuleHandle handle);

private:
  SimpleLayerSpec::ObjectLayer object_layer;
};
