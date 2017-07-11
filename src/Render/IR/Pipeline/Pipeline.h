#pragma once

#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/IR/Value.h"

#include <string>
#include <vector>


class IRRenderer;
struct FunctionNode;
class ModuleHandle;

typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;


class Pipeline {
public:
  virtual void process_function_node(FunctionNode *node) = 0;

  virtual llvm::JITSymbol find_symbol(const std::string &name) = 0;
  virtual llvm::JITSymbol find_unmangled_symbol(const std::string &name) = 0;

  virtual void flush_modules(ModuleSet &modules) = 0;

protected:
  Pipeline(IRRenderer *renderer) : renderer(renderer) {};
  IRRenderer *renderer;
};
