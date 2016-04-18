#pragma once

#include "llvm/ExecutionEngine/Orc/OrcArchitectureSupport.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"

#include "SessionContext.h"


class JITHelper {
public:
  typedef IRCompileLayer<ObjectLinkingLayer<>> CompileLayer;
  typedef LazyEmittingLayer<CompileLayer> LazyEmitLayer;
  typedef LazyEmitLayer::ModuleSetHandleT ModuleHandle;

  JITHelper(SessionContext &session);

  std::string mangle(const std::string &Name);

  void add_function(std::unique_ptr<FunctionNode> node);

  ModuleHandle add_module(std::unique_ptr<llvm::Module> module);
  void remove_module(ModuleHandle handle);

  JITSymbol find_symbol(const std::string &name);
  JITSymbol find_symbol_in(ModuleHandle handle, const std::string &name);
  JITSymbol find_unmangled_symbol(const std::string &name);
  JITSymbol find_unmangled_symbol_in(ModuleHandle handle, const std::string &name);

private:

  // This method searches the FunctionDefs map for a definition of 'Name'. If it
  // finds one it generates a stub for it and returns the address of the stub.
  RuntimeDyld::SymbolInfo search_functions(const std::string &name);

  // This method will take the AST for a function definition and IR-gen a stub
  // for that function that will, on first call, IR-gen the actual body of the
  // function.
  ModuleHandle generate_stub(std::unique_ptr<FunctionNode> node);

  SessionContext &session;
  SectionMemoryManager CCMgrMemMgr;
  ObjectLinkingLayer<> object_layer;
  CompileLayer compile_layer;
  LazyEmitLayer lazy_emit_layer;

  std::map<std::string, std::unique_ptr<FunctionNode>> functions;

  LocalJITCompileCallbackManager<OrcX86_64> compile_callbacks;
};
