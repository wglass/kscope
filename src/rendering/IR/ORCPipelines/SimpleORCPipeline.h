#pragma once

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"

#include "ORCPipeline.h"


typedef ObjectLinkingLayer<> SimpleObjectLayer;
typedef IRCompileLayer<SimpleObjectLayer> SimpleCompileLayer;
typedef SimpleCompileLayer::ModuleSetHandleT SimpleModuleHandle;


class SimpleORCPipeline : ORCPipeline<SimpleModuleHandle> {
public:
  SimpleModuleHandle add_modules(std::unique_ptr<llvm::Module> module);
  void remove_modules(SimpleModuleHandle handle);

  JITSymbol find_symbol(const std::string&name);
  JITSymbol find_symbol_in(SimpleModuleHandle handle, const std::string &name);

private:
  SimpleObjectLayer object_layer;
  SimpleCompileLayer compile_layer;

  const llvm::DataLayout data_layout;
};
