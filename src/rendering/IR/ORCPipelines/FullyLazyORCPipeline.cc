#include "FullyLazyORCPipeline.h"

#include "ast/FunctionNode.h"

#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"

#include <string>


JITSymbol
FullyLazyORCPipeline::find_symbol(const std::string &name) {
  emit_layer.findSymbol(name, false);
}

JITSymbol
FullyLazyORCPipeline::find_symbol_in(FullyLazyModuleHandle handle,
                                   const std::string &name) {
  emit_layer.findSymbolIn(handle, name, false);
}

FullyLazyModuleHandle
FullyLazyORCPipeline::add_modules(std::vector<llvm::Module *> modules) {
  // We need a memory manager to allocate memory and resolve symbols for this
  // new module. Create one that resolves symbols by looking back into the
  // JIT.
  auto resolver = createLambdaResolver(
    [&](const std::string &name) {
      if (auto symbol = find_symbol(name)) {
        return RuntimeDyld::SymbolInfo(symbol.getAddress(), symbol.getFlags());
      } else {
        return search_functions(name);
      }
    },
    [](const std::string &name) {
      return nullptr;
    }
  );

  return emit_layer.addModuleSet(modules,
                                 make_unique<SectionMemoryManager>(),
                                 std::move(resolver));
}

void
FullyLazyORCPipeline::remove_module(FullyLazyModuleHandle handle) {
  emit_layer.removeModuleSet(handle);
}

RuntimeDyld::SymbolInfo
FullyLazyORCPipeline::search_functions(const std::string &name) {
  auto iter = functions.find(name);
  if (iter == functions.end()) {
      return nullptr;
  }

  auto function_node = std::move(iter->second);
  functions.erase(iter);

  auto handle = generate_stub(std::move(function_node));
  auto symbol = find_symbol_in(handle, name);

  return RuntimeDyld::SymbolInfo(symbol.getAddress(), symbol.getFlags());
}

FullyLazyModuleHandle
FullyLazyORCPipeline::generate_stub(std::unique_ptr<FunctionNode> node) {
  // Step 1) IRGen a prototype for the stub. This will have the same type as
  //         the function.
  IRGenContext C(Session);
  llvm::Function *func = render(node->proto);

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
    createImplPointer(*func->getType(),
                      *func->getParent(),
                      body_ptr_name,
                      createIRTypedAddress(*func->getFunctionType(),
                                           CallbackInfo.getAddress()));

  makeStub(*func, *body_ptr);

  // Step 4) Add the module containing the stub to the JIT.
  auto stub = add_module(C.takeM());

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
  std::shared_ptr<FunctionNode> shared_node = std::move(node);

  callback_info.setCompileAction(
    [this, shared_node, body_ptr_name, stub]() {
      auto module = render_func_to_module(*shared_node);
      auto handle = add_module(module);
      auto BodySym = find_unmangled_symbol_in(handle, shared_node->proto->name);
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

std::unique_ptr<llvm::Module>
FullyLazyORCPipeline::render_func_to_module(const FunctionNode &node) {
  IRGenContext C(S);
  auto function = render(node);
  if (!function) {
    return nullptr;
  }

  return C.takeM()
}
