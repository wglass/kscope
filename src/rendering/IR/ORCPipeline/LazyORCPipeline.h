#pragma once

#include "ORCPipeline.h"

#include "ast/FunctionNode.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/OrcArchitectureSupport.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/IR/Module.h"


typedef llvm::orc::ObjectLinkingLayer<> ObjectLayer;
typedef llvm::orc::IRCompileLayer<ObjectLayer> CompileLayer;
typedef llvm::orc::LazyEmittingLayer<CompileLayer> EmitLayer;

class LazyORCPipeline : public ORCPipeline<EmitLayer> {

public:
  typedef ORCPipeline<EmitLayer>::ModuleSet ModuleSet;
  typedef EmitLayer::ModuleSetHandleT ModuleHandle;

  LazyORCPipeline(IRRenderer *renderer);

  void add_function(FunctionNode *node);

  ModuleHandle add_modules(ModuleSet modules);
  void remove_modules(ModuleHandle handle);

  llvm::orc::JITSymbol find_symbol(const std::string &name);
  llvm::orc::JITSymbol find_symbol_in(ModuleHandle handle,
                                      const std::string &name);

private:
  ObjectLayer object_layer;
  CompileLayer compile_layer;
  EmitLayer emit_layer;

  llvm::orc::LocalJITCompileCallbackManager<llvm::orc::OrcX86_64> compile_callbacks;

  std::map<std::string, FunctionNode *> functions;

  llvm::RuntimeDyld::SymbolInfo search_functions(const std::string &name);
  ModuleHandle generate_stub(FunctionNode *node);
};
