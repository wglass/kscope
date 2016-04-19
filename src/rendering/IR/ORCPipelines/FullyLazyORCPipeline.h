#pragma once

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/OrcArchitectureSupport.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/IR/Module.h"

#include "ast/FunctionNode.h"

#include "ORCPipeline.h"


typedef OjectLinkingLayer<> FullyLazyObjectLayer;
typedef IRCompileLayer<FullyLazyObjectLayer> FullyLazyCompileLayer;
typedef LazyEmittingLayer<FullyLazyCompileLayer> FullyLazyEmitLayer;
typedef FullyLazyEmitLayer::ModuleSetHandleT FullyLazyModuleHandle;

class FullyLazyORCPipeline : ORCPipeline<FullyLazyModuleHandle> {

public:
  FullyLazyModuleHandle add_modules(std::vector<llvm:Module *> modules);
  void remove_modules(FullyLazyModuleHandle handle);

  JITSymbol find_symbol(const std::string &name);
  JITSymbol find_symbol_in(FullyLazyModuleHandle handle, const std::string &name);

private:
  FullyLazyObjectLayer object_layer;
  FullyLazyCompileLayer compile_layer;
  FullyLazyEmitLayer emit_layer;

  LocalJITCompileCallbackManager<OrcX86_64> compile_callbacks;

  std::map<std::string, std::unique_ptr<FunctionNode>> functions;

  RuntimeDyld::SymbolInfo search_functions(const std::string &name);
  FullyLazyModuleHandle generate_stub(std::unique_ptr<FunctionNode> node);
  std::unique_ptr<llvm::Module> render_func_to_module(const FunctionNode &node);
};
