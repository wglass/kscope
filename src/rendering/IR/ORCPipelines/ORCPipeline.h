#pragma once

#include <string>

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"


template <class ModuleHandle>
class ORCPipeline {

public:
  ModuleHandle add_modules(std::vector<llvm:Module *> modules);
  void remove_modules(ModuleHandle handle);

  ModuleHandle add_module(std::unique_ptr<llvm::Module> module) {
    std::vector<llvm::Module *> modules;
    modules.push_back(std::move(module));
    add_modules(modules);
  }

  JITSymbol find_symbol(const std::string &name);
  JITSymbol find_symbol_in(ModuleHandle handle, const std::string &name);

  std::string mangle(const std::string &name) {
    std::string mangled_name;
    {
      llvm::raw_string_ostream stream(mangled_name);
      llvm::Mangler::getNameWithPrefix(stream, name, data_layout);
    }
    return mangled_name;
  }

  JITSymbol find_unmangled_symbol(const std::string &name) {
    find_symbol(mangle(name));
  }
  JITSymbol find_unmangled_symbol_in(ModuleHandle handle,
                                     const std::string &name) {
    find_symbol_in(handle, mangle(name));
  }

};
