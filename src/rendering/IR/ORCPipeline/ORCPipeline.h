#pragma once

#include "ast/ASTNode.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <string>
#include <set>


class IRRenderer;
struct FunctionNode;

template<class TopLayer>
class ORCPipeline {

public:
  typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;
  typedef typename TopLayer::ModuleSetHandleT ModuleHandle;

  ORCPipeline(IRRenderer *renderer)
    : renderer(renderer),
      target_machine(llvm::EngineBuilder().selectTarget()),
      data_layout(target_machine->createDataLayout()) {}

  virtual void add_function(FunctionNode *node) = 0;

  ModuleHandle add_modules(ModuleSet modules);
  void remove_modules(ModuleHandle handle);

  virtual llvm::orc::JITSymbol find_symbol(const std::string &name) = 0;
  virtual llvm::orc::JITSymbol find_symbol_in(ModuleHandle handle,
                                              const std::string &name) = 0;

  std::string mangle(const std::string &name) {
    std::string mangled_name;
    {
      llvm::raw_string_ostream stream(mangled_name);
      llvm::Mangler::getNameWithPrefix(stream, name, data_layout);
    }
    return mangled_name;
  }

  llvm::orc::JITSymbol find_unmangled_symbol(const std::string &name) {
    return find_symbol(mangle(name));
  }
  llvm::orc::JITSymbol find_unmangled_symbol_in(ModuleHandle handle,
                                                const std::string &name) {
    return find_symbol_in(handle, mangle(name));
  }

  IRRenderer *renderer;
  std::unique_ptr<llvm::TargetMachine> target_machine;

private:
  const llvm::DataLayout data_layout;
};
