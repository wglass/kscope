#include "LazyORCPipeline.h"

#include "Render/IR/IRRenderer.h"

#include "kscope/AST/FunctionNode.h"

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
  compile_callbacks((reinterpret_cast<uintptr_t>(handle_address_error))) {}

void
LazyORCPipeline::process_function_node(FunctionNode *node) {
  functions[mangle(node->proto->name)] = node;
}

llvm::orc::JITSymbol
LazyORCPipeline::find_symbol(const std::string &name) {
  auto symbol = top_layer->findSymbol(name, false);

  if ( ! symbol ) {
    symbol = search_functions(name);
  }

  return symbol;
}

LazyORCPipeline::ModuleHandle
LazyORCPipeline::add_modules(ModuleSet &modules) {
  auto resolver = llvm::orc::createLambdaResolver(
    [&](const std::string &name) {
      if ( auto symbol = find_symbol(name) ) {
        return llvm::RuntimeDyld::SymbolInfo(symbol.getAddress(),
                                             symbol.getFlags());
      }
      if ( auto addr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(name) ) {
        return llvm::RuntimeDyld::SymbolInfo(addr,
                                             llvm::JITSymbolFlags::Exported);
      }

      return llvm::RuntimeDyld::SymbolInfo(nullptr);
    },
    [](const std::string &name) {
      return nullptr;
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

llvm::orc::JITSymbol
LazyORCPipeline::search_functions(const std::string &name) {
  auto search = functions.find(name);
  if ( search == functions.end() ) {
      return nullptr;
  }

  auto function_node = search->second;
  functions.erase(search);

  generate_stub(function_node);

  return find_symbol_in(previous_flush, name);
}

void
LazyORCPipeline::generate_stub(FunctionNode *node) {
  auto *proto = static_cast<llvm::Function*>(renderer->visit(node->proto));
  auto callback_info = compile_callbacks.getCompileCallback();

  auto body_ptr_name = (proto->getName() + "$address").str();
  auto *body_ptr = llvm::orc::createImplPointer(*proto->getType(),
                                                *proto->getParent(),
                                                body_ptr_name,
                                                llvm::orc::createIRTypedAddress(*proto->getFunctionType(),
                                                                                callback_info.getAddress()));

  llvm::orc::makeStub(*proto, *body_ptr);

  renderer->flush_modules();

  auto stub_handle = std::move(previous_flush);
  callback_info.setCompileAction(
    [this, node, body_ptr_name, stub_handle]() {
      renderer->render_function(node);
      renderer->flush_modules();

      auto symbol = find_unmangled_symbol_in(previous_flush, node->proto->name);
      auto stub_symbol = find_unmangled_symbol_in(stub_handle, body_ptr_name);

      auto body_address = symbol.getAddress();
      auto stub_pointer = reinterpret_cast<void*>(static_cast<uintptr_t>(stub_symbol.getAddress()));

      memcpy(stub_pointer, &body_address, sizeof(uintptr_t));

      return body_address;
    }
  );
}
