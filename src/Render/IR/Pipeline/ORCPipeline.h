#pragma once

#include "Pipeline.h"

#include "Render/IR/IRRenderer.h"

#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <memory>
#include <string>
#include <set>


class IRRenderer;
struct FunctionNode;


template<class Subclass, class LayerSpec>
class ORCPipeline : public Pipeline {
public:
  typedef typename LayerSpec::TopLayer::ModuleSetHandleT ModuleHandle;
  typedef typename LayerSpec::TopLayer TopLayer;

  ORCPipeline<LayerSpec>(IRRenderer *renderer,
                         std::unique_ptr<TopLayer> top_layer)
  : Pipeline(renderer),
    top_layer(std::move(top_layer)) {}

  virtual ModuleHandle add_modules(ModuleSet &modules) = 0;
  virtual void remove_modules(ModuleHandle handle) = 0;

  void flush_modules(ModuleSet &modules) {
    previous_flush = add_modules(modules);
  }

  std::string mangle(const std::string &name) {
    std::string mangled_name;
    {
      llvm::raw_string_ostream stream(mangled_name);
      llvm::Mangler::getNameWithPrefix(stream, name, renderer->get_data_layout());
    }
    return mangled_name;
  }

  llvm::JITSymbol find_unmangled_symbol(const std::string &name) {
    return find_symbol(mangle(name));
  }
  llvm::JITSymbol find_symbol(const std::string &name) {
    fprintf(stderr, "Looking up symbol in ORCPipeline: %s\n", name.c_str());
    fprintf(stderr, "location of top_layer: %p\n", &top_layer);
    return top_layer->findSymbol(name, false);
  }
  llvm::JITSymbol find_unmangled_symbol_in(ModuleHandle handle,
                                                const std::string &name) {
    return find_symbol_in(handle, mangle(name));
  }
  llvm::JITSymbol find_symbol_in(ModuleHandle handle,
                                      const std::string &name) {
    return top_layer->findSymbolIn(handle, name, false);
  }

protected:
  ModuleHandle previous_flush;
  std::unique_ptr<TopLayer> top_layer;
};
