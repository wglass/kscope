#include "llvm/ExecutionEngine/Orc/OrcArchitectureSupport.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Mangler.h"

#include "SessionContext.h"

#include "JITHelper.h"


JITHelper::JITHelper(SessionContext &session)
  : Session(session),
    compile_layer(ObjectLayer, SimpleCompiler(session.getTarget())),
    lazy_emit_layer(compile_layer),
    compile_callbacks(reinterpret_cast<uintptr_t>(EarthShatteringKaboom)) {};

std::string
JITHelper::mangle(const std::string &name) {
std::string MangledName;
{
raw_string_ostream MangledNameStream(MangledName);
Mangler::getNameWithPrefix(MangledNameStream, Name,
                             Session.getTarget().createDataLayout());
}
return MangledName;
}

JITSymbol
JITHelper::find_symbol(const std::string &name) {
  return LazyEmitLayer.findSymbol(name, false);
}

JITSymbol
JITHelper::find_symbol_in(ModuleHandle handle, const std::string &name) {
  return LazyEmitLayer.findSymbolIn(handle, name, false);
}

JITSymbol
JITHelper::find_unmangled_symbol(const std::string &name) {
  return find_symbol(mangle(name));
}

JITSymbol
JITHelper::find_unmangled_symbol_in(ModuleHandle handle, const std::string &name) {
    return find_symbol_in(handle, mangle(name));
}


void
JITHelper::add_function(std::unique_ptr<FunctionNode> node) {
functions[mangle(ndoe->proto->name)] = std::move(node);
}

ModuleHandle
JITHelper::add_module(std::unique_ptr<llvm::Module> module) {
    // We need a memory manager to allocate memory and resolve symbols for this
    // new module. Create one that resolves symbols by looking back into the
    // JIT.
    auto resolver = createLambdaResolver(
      [&](const std::string &name) {
        // First try to find 'Name' within the JIT.
        if (auto symbol = find_symbol(name)) {
          return RuntimeDyld::SymbolInfo(symbol.getAddress(),
                                         symbol.getFlags());
        }
        // If we don't already have a definition of 'Name' then search
        // the ASTs.
        return search_functions(name);
      },
      [](const std::string &s) {
        return nullptr;
      }
    );

    return lazy_emit_layer.addModuleSet(singletonSet(std::move(module)),
                                        make_unique<SectionMemoryManager>(),
                                        std::move(resolver));
}

void
JITHelper::remove_module(ModuleHandle handle) {
  lazy_emit_layer.removeModuleSet(handle);
}

RuntimeDyld::SymbolInfo
JITHelper::search_functions(const std::string &name) {
  auto iter = functions.find(name);
  if (iter == functions.end()) {
    return nullptr;
  }

  // Return the address of the stub.
  // Take the FunctionAST out of the map.
  auto function_node = std::move(iter->second);
  functions.erase(iter);

  // IRGen the AST, add it to the JIT, and return the address for it.
  auto handler = irGenStub(std::move(function_node));
  auto symbol = find_symbol_in(handler, name);

  return RuntimeDyld::SymbolInfo(symbol.getAddress(), symbol.getFlags());
}

ModuleHandle
JITHelper::generate_stub(std::unique_ptr<FunctionNode> node) {
    // Step 1) IRGen a prototype for the stub. This will have the same type as
    //         the function.
    IRGenContext irgen_context(session);
    llvm::Function *function = render(node->proto);

    // Step 2) Get a compile callback that can be used to compile the body of
    //         the function. The resulting CallbackInfo type will let us set the
    //         compile and update actions for the callback, and get a pointer to
    //         the jit trampoline that we need to call to trigger those actions.
    auto callback_info = compile_callbacks.getCompileCallback();

    // Step 3) Create a stub that will indirectly call the body of this
    //         function once it is compiled. Initially, set the function
    //         pointer for the indirection to point at the trampoline.
    std::string body_ptr_name = (function->getName() + "$address").str();
    llvm::GlobalVariable *body_ptr = createImplPointer(*function->getType(),
                                                       *function->getParent(),
                                                       body_ptr_name,
                                                       createIRTypedAddress(*function->getFunctionType(),
                                                                            callback_info.getAddress()));
    makeStub(*function, *body_ptr);

    // Step 4) Add the module containing the stub to the JIT.
    auto stub_handler = add_module(irgen_context.take_module());

    // Step 5) Set the compile and update actions.
    //
    //   The compile action will IRGen the function and add it to the JIT, then
    // request its address, which will trigger codegen. Since we don't need the
    // AST after this, we pass ownership of the AST into the compile action:
    // compile actions (and update actions) are deleted after they're run, so
    // this will free the AST for us.
    //
    //   The update action will update FunctionBodyPointer to point at the newly
    // compiled function.
    std::shared_ptr<FunctionNode> shared_node = std::move(node);

    callback_info.setCompileAction([this, shared_node, body_ptr_name, stub_handler]() {
      auto handler = add_module(IRGen(session, *shared_node));
      auto body_symbol = find_unmangled_symbol_in(handler, shared_node->proto->name);
      auto body_ptr_symbol = find_unmangled_symbol_in(stub_handler, body_ptr_name);

      auto body_addr = BodySym.getAddress();
      auto body_ptr = reinterpret_cast<void*>(
                       static_cast<uintptr_t>(body_ptr_symbol.getAddress()));

      memcpy(body_ptr, &body_addr, sizeof(uintptr_t));
      return body_addr;
    });

    return stub_handler;
}


static std::unique_ptr<llvm::Module>
IRGen(SessionContext &session, const FunctionNode &node) {
  IRGenContext context(session);
  auto result = render(node);
  if (!result) {
    return nullptr;
  }

  return context.take_module();
}
