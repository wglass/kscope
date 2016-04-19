#include "SimpleORCPipeline.h"

#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/IR/Module.h"

#include <string>


JITSymbol
SimpleORCPipeline::find_symbol(const std::string &name) {
  return compile_layer.findSymbol(name, true);
}

JITsymbol
SimpleORCPipeline::find_symbol_in(SimpleModuleHandle handle,
                                const std::string &name) {
  return compile_layer.findSymbolIn(handle, name, true);
}

SimpleModuleHandle
SimpleORCPipeline::add_modules(std::vector<llvm::Module *> modules) {
  auto Resolver = createLambdaResolver(
    [&](const std::string &name) {
      if (auto symbol = find_symbol(name)) {
        return RuntimeDyld::SymbolInfo(symbol.getAddress(), symbol.getFlags());
      } else {
        return RuntimeDyld::SymbolInfo(nullptr);
      }
    },
    [](const std::string &name) {
      return nullptr;
    }
  );
  return compile_layer.addModuleSet(modules,
                                    make_unique<SectionMemoryManager>(),
                                    std::move(resolver));
}

void
SimpleORCPipeline::remove_modules(SimpleModuleHandle handle) {
  compile_layer.removeModuleSet(handle);
}
