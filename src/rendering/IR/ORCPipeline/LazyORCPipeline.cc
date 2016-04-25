#include "LazyORCPipeline.h"

#include "ast/FunctionNode.h"
#include "rendering/IR/IRRenderer.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"

#include <functional>
#include <string>


static void handle_address_error() {
  fprintf(stderr, "Address error in compile callback!");
  exit(1);
}

LazyORCPipeline::LazyORCPipeline(IRRenderer *renderer)
  : ORCPipeline<EmitLayer>::ORCPipeline(renderer),
    compile_layer(object_layer, llvm::orc::SimpleCompiler(*target_machine)),
    emit_layer(compile_layer),
    compile_callbacks((reinterpret_cast<uintptr_t>(handle_address_error))) {}

llvm::orc::JITSymbol
LazyORCPipeline::find_symbol(const std::string &name) {
  fprintf(stderr, "Finding (mangled) symbol: %s\n", name.c_str());
  return emit_layer.findSymbol(name, false);
}

llvm::orc::JITSymbol
LazyORCPipeline::find_symbol_in(LazyORCPipeline::ModuleHandle handle,
                                const std::string &name) {
  return emit_layer.findSymbolIn(handle, name, false);
}

void
LazyORCPipeline::add_function(FunctionNode *node) {
  functions[mangle(node->proto->name)] = node;
}

LazyORCPipeline::ModuleHandle
LazyORCPipeline::add_modules(LazyORCPipeline::ModuleSet modules) {
  // We need a memory manager to allocate memory and resolve symbols for this
  // new module. Create one that resolves symbols by looking back into the
  // JIT.
  auto resolver = llvm::orc::createLambdaResolver(
    [&](const std::string &name) {
      fprintf(stderr, "In resolver, looking for %s\n", name.c_str());
      if (auto symbol = find_symbol(name)) {
        return llvm::RuntimeDyld::SymbolInfo(symbol.getAddress(),
                                             symbol.getFlags());
      } else {
        return search_functions(name);
      }
    },
    [](const std::string &name) {
      return nullptr;
    }
  );

  fprintf(stderr, "addModuleSet for %lu modules\n", modules.size());
  return emit_layer.addModuleSet(std::move(modules),
                                 std::make_unique<llvm::SectionMemoryManager>(),
                                 std::move(resolver));
}

void
LazyORCPipeline::remove_modules(LazyORCPipeline::ModuleHandle handle) {
  emit_layer.removeModuleSet(handle);
}

llvm::RuntimeDyld::SymbolInfo
LazyORCPipeline::search_functions(const std::string &name) {
  fprintf(stderr, "Looking for function %s", name.c_str());
  auto iter = functions.find(name);
  if (iter == functions.end()) {
      return nullptr;
  }

  auto function_node = iter->second;
  functions.erase(iter);

  auto handle = generate_stub(function_node);
  auto symbol = find_symbol_in(handle, name);

  return llvm::RuntimeDyld::SymbolInfo(symbol.getAddress(), symbol.getFlags());
}

LazyORCPipeline::ModuleHandle
LazyORCPipeline::generate_stub(FunctionNode *node) {
  llvm::Function *func = renderer->render_node(node->proto);

  // Step 2) Get a compile callback that can be used to compile the body of
  //         the function. The resulting CallbackInfo type will let us set the
  //         compile and update actions for the callback, and get a pointer to
  //         the jit trampoline that we need to call to trigger those actions.
  auto callback_info = compile_callbacks.getCompileCallback();

  // Step 3) Create a stub that will indirectly call the body of this
  //         function once it is compiled. Initially, set the function
  //         pointer for the indirection to point at the trampoline.
  std::string body_ptr_name = (func->getName() + "$address").str();
  llvm::GlobalVariable *body_ptr =
    llvm::orc::createImplPointer(*func->getType(),
                                 *func->getParent(),
                                 body_ptr_name,
                                 llvm::orc::createIRTypedAddress(*func->getFunctionType(),
                                                                 callback_info.getAddress()));

  llvm::orc::makeStub(*func, *body_ptr);

  // Step 4) Add the module containing the stub to the JIT.
  auto stub = renderer->flush_modules();

  // Step 5) Set the compile and update actions.
  //
  //   The compile action will IRGen the function and add it to the JIT, then
  // request its address, which will trigger codegen. Since we don't need the
  // AST after this, we pass ownership of the AST into the compile action:
  // compile actions (and update actions) are deleted after they're run, so
  // this will free the AST for us.
  //
  //   The update action will update body_ptr to point at the newly
  // compiled function.

  callback_info.setCompileAction(
    [this, node, body_ptr_name, stub]() {
      renderer->render_node(node);
      auto handle = renderer->flush_modules();
      auto BodySym = find_unmangled_symbol_in(handle, node->proto->name);
      auto BodyPtrSym = find_unmangled_symbol_in(stub, body_ptr_name);

      auto BodyAddr = BodySym.getAddress();
      auto BodyPtr = reinterpret_cast<void*>(
                     static_cast<uintptr_t>(BodyPtrSym.getAddress()));

      memcpy(BodyPtr, &BodyAddr, sizeof(uintptr_t));

      return BodyAddr;
    }
  );

  return stub;
}
