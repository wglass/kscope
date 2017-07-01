#include "LazyORCPipeline.h"

#include "Render/IR/IRRenderer.h"

#include "kscope/AST/FunctionNode.h"

#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Error.h"

#include <functional>
#include <string>
#include <cstdio>


static void handle_address_error() {
  fprintf(stderr, "Address error in compile callback!");
  exit(1);
}

LazyORCPipeline::LazyORCPipeline(IRRenderer *renderer)
  : ORCPipeline<LazyORCPipeline, LazyLayerSpec>(renderer,
                                                std::make_unique<LazyLayerSpec::TopLayer>(compile_layer)),
  compile_layer(object_layer, llvm::orc::SimpleCompiler(renderer->get_target_machine())),
  target_machine(llvm::EngineBuilder().selectTarget()),
  data_layout(target_machine->createDataLayout()),
  callback_manager(llvm::orc::createLocalCompileCallbackManager(target_machine->getTargetTriple(), 0)) {
  auto stub_manager_builder = llvm::orc::createLocalIndirectStubsManagerBuilder(target_machine->getTargetTriple());

  stub_manager = stub_manager_builder();
}

LazyORCPipeline::ModuleHandle
LazyORCPipeline::add_modules(ModuleSet &modules) {
  auto resolver = llvm::orc::createLambdaResolver(
    [&](const std::string &name) {
      if ( auto symbol = stub_manager->findStub(name, false) ) {
        return symbol;
      } else if ( auto symbol = find_unmangled_symbol(name) ) {
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
LazyORCPipeline::remove_modules(LazyORCPipeline::ModuleHandle handle) {
  top_layer->removeModuleSet(handle);
}

void
LazyORCPipeline::process_function_node(FunctionNode *node) {
  auto callback_info = callback_manager->getCompileCallback();

  auto err = stub_manager->createStub(mangle(node->proto->name),
                                      callback_info.getAddress(),
                                      llvm::JITSymbolFlags::Exported);
  if ( err ) {
    logAllUnhandledErrors(std::move(err), llvm::errs(),
                          "Error updating function pointer: ");
    exit(1);
  }

  callback_info.setCompileAction(
    [this, node]() {
      auto function = renderer->render_function(node);
      function->setName(function->getName() + "$impl");
      renderer->flush_modules();

      auto symbol = find_unmangled_symbol(node->proto->name + "$impl");
      auto address = symbol.getAddress();

      auto err = stub_manager->updatePointer(mangle(node->proto->name), address);
      if ( err ) {
        logAllUnhandledErrors(std::move(err), llvm::errs(),
                              "Error updating function pointer: ");
          exit(1);
      }

      return address;
    }
  );
}
