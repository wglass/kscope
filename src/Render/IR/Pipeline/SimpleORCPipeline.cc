#include "SimpleORCPipeline.h"

#include "Render/IR/IRRenderer.h"

#include "kscope/AST/FunctionNode.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Module.h"

#include <memory>
#include <string>


SimpleORCPipeline::SimpleORCPipeline(IRRenderer *renderer)
  : ORCPipeline<SimpleORCPipeline, SimpleLayerSpec>(renderer,
                                                    std::make_unique<SimpleLayerSpec::TopLayer>(object_layer,
                                                                                                llvm::orc::SimpleCompiler(*llvm::EngineBuilder().selectTarget()))) { }

void
SimpleORCPipeline::process_function_node(FunctionNode *node) {
  renderer->render_function(node);
}

SimpleORCPipeline::ModuleHandle
SimpleORCPipeline::add_modules(ModuleSet &modules) {
  auto resolver = llvm::orc::createLambdaResolver(
    [&](const std::string &name) {
      if (auto symbol = find_symbol(name)) {
        return symbol;
      } else {
        return llvm::JITSymbol(nullptr);
      }
    },
    [](const std::string &name) {
      if ( auto addr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(name) ) {
        return llvm::JITSymbol(addr, llvm::JITSymbolFlags::Exported);
      } else {
        return llvm::JITSymbol(nullptr);
      }
    }
  );
  return top_layer->addModuleSet(std::move(modules),
                                 std::make_unique<llvm::SectionMemoryManager>(),
                                 std::move(resolver));
}

void
SimpleORCPipeline::remove_modules(SimpleORCPipeline::ModuleHandle handle) {
  top_layer->removeModuleSet(handle);
}
