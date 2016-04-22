#include "SimpleORCPipeline.h"

#include "ast/FunctionNode.h"
#include "rendering/IR/IRRenderer.h"

#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/IR/Module.h"

#include <string>


llvm::orc::JITSymbol
SimpleORCPipeline::find_symbol(const std::string &name) {
  return compile_layer.findSymbol(name, true);
}

llvm::orc::JITSymbol
SimpleORCPipeline::find_symbol_in(ModuleHandle handle,
                                const std::string &name) {
  return compile_layer.findSymbolIn(handle, name, true);
}

void
SimpleORCPipeline::add_function(FunctionNode *node) {
  renderer->render_function(node);
  renderer->flush_modules();
}

SimpleORCPipeline::ModuleHandle
SimpleORCPipeline::add_modules(SimpleORCPipeline::ModuleSet modules) {
  auto resolver = llvm::orc::createLambdaResolver(
    [&](const std::string &name) {
      if (auto symbol = find_symbol(name)) {
        return llvm::RuntimeDyld::SymbolInfo(symbol.getAddress(),
                                             symbol.getFlags());
      } else {
        return llvm::RuntimeDyld::SymbolInfo(nullptr);
      }
    },
    [](const std::string &name) {
      return nullptr;
    }
  );
  return compile_layer.addModuleSet(std::move(modules),
                                    std::make_unique<llvm::SectionMemoryManager>(),
                                    std::move(resolver));
}

void
SimpleORCPipeline::remove_modules(SimpleORCPipeline::ModuleHandle handle) {
  compile_layer.removeModuleSet(handle);
}
