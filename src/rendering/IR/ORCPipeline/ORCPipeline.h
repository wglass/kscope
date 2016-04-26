#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <string>
#include <set>


template <class Pipeline> class IRRenderer;
struct FunctionNode;


typedef std::vector<std::unique_ptr<llvm::Module> > ModuleSet;


template<class Subclass, class LayerSpec>
class ORCPipeline {
public:
  typedef typename LayerSpec::TopLayer::ModuleSetHandleT ModuleHandle;

  virtual void add_function(FunctionNode *node) = 0;

  virtual ModuleHandle add_modules(ModuleSet modules);
  virtual void remove_modules(ModuleHandle handle);

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
  llvm::orc::JITSymbol find_symbol(const std::string &name) {
    return top_layer.findSymbol(name, false);
  }
  llvm::orc::JITSymbol find_unmangled_symbol_in(ModuleHandle handle,
                                                const std::string &name) {
    return find_symbol_in(handle, mangle(name));
  }
  llvm::orc::JITSymbol find_symbol_in(ModuleHandle handle,
                                      const std::string &name) {
    return top_layer.findSymbolIn(handle, name, false);
  }

  IRRenderer<Subclass> *renderer;
  std::unique_ptr<llvm::TargetMachine> target_machine;
  typename LayerSpec::TopLayer top_layer;

protected:
  ORCPipeline<LayerSpec>(IRRenderer<Subclass> *renderer,
                           typename LayerSpec::TopLayer top_layer)
  : renderer(renderer),
    target_machine(llvm::EngineBuilder().selectTarget()),
    top_layer(std::move(top_layer)),
    data_layout(target_machine->createDataLayout()) {}

private:
  const llvm::DataLayout data_layout;
};
