#pragma once

#include "ORCPipeline.h"

#include "ast/FunctionNode.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"


typedef llvm::orc::ObjectLinkingLayer<> SimpleObjectLayer;
typedef llvm::orc::IRCompileLayer<SimpleObjectLayer> SimpleCompileLayer;


class SimpleORCPipeline : ORCPipeline<SimpleCompileLayer> {
public:
  typedef ORCPipeline<SimpleCompileLayer>::ModuleSet ModuleSet;
  typedef SimpleCompileLayer::ModuleSetHandleT ModuleHandle;

  void add_function(FunctionNode *node);

  ModuleHandle add_modules(ModuleSet module);
  void remove_modules(ModuleHandle handle);

  llvm::orc::JITSymbol find_symbol(const std::string&name);
  llvm::orc::JITSymbol find_symbol_in(ModuleHandle handle,
                                      const std::string &name);

private:
  SimpleObjectLayer object_layer;
  SimpleCompileLayer compile_layer;
};
