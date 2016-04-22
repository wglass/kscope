#include "IRRenderContext.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"


static int module_id = 1;

static std::string
generate_module_name() {
  std::string name = "llvm_module_";
  name += std::to_string(module_id);
  module_id++;
  return name;
}

IRRenderContext::IRRenderContext(llvm::LLVMContext &llvm_context)
  : llvm_context(llvm_context),
    module(std::make_unique<llvm::Module>(generate_module_name(), llvm_context)) {}


IRRenderContext::IRRenderContext(llvm::LLVMContext &llvm_context,
                                 std::unique_ptr<llvm::Module> module)
  : llvm_context(llvm_context),
    module(std::move(module)),
    builder(std::make_unique<llvm::IRBuilder<>>(llvm_context)),
    pass_manager(std::make_unique<llvm::legacy::FunctionPassManager>(module.get())) {

  pass_manager->add(llvm::createPromoteMemoryToRegisterPass());
  pass_manager->add(llvm::createInstructionCombiningPass());
  pass_manager->add(llvm::createReassociatePass());
  pass_manager->add(llvm::createGVNPass());
  pass_manager->add(llvm::createCFGSimplificationPass());

  pass_manager->doInitialization();
}

IRRenderContext::IRRenderContext(IRRenderContext &&other)
  : llvm_context(other.llvm_context) {
  module = std::move(other.module);
  builder = std::move(other.builder);
  pass_manager = std::move(other.pass_manager);
}

IRRenderContext::~IRRenderContext() {
  module.release();
  builder.release();
  pass_manager.release();
}

llvm::LLVMContext &
IRRenderContext::get_llvm_context() {
  return llvm_context;
}

llvm::Module &
IRRenderContext::get_module() {
  return *module;
}

llvm::IRBuilder<> &
IRRenderContext::get_builder() {
  return *builder;
}

llvm::legacy::FunctionPassManager &
IRRenderContext::get_pass_manager() {
  return *pass_manager;
}

bool
IRRenderContext::has_module() {
  if ( ! module ) {
    return false;
  } else {
    return true;
  }
}

std::unique_ptr<llvm::Module>
IRRenderContext::give_up_module() {
  std::unique_ptr<llvm::Module> mod = std::move(module);

  builder.release();
  pass_manager.release();

  return mod;
}

llvm::AllocaInst *
IRRenderContext::get_named_value (const std::string &name){
    return named_values[name];
}

void
IRRenderContext::set_named_value(const std::string &name,
                                 llvm::AllocaInst *value) {
    named_values[name] = value;
}

void
IRRenderContext::clear_named_value(const std::string &name) {
    named_values.erase(name);
}

void
IRRenderContext::clear_all_named_values() {
    named_values.clear();
}

llvm::AllocaInst *
IRRenderContext::create_entry_block_alloca(llvm::Function *func,
                                           const std::string &name) {
  llvm::IRBuilder<> tmp_builder(&func->getEntryBlock(),
                                func->getEntryBlock().begin());

  return tmp_builder.CreateAlloca(llvm::Type::getDoubleTy(llvm_context),
                                  0,
                                  name.c_str());
}

void
IRRenderContext::create_argument_allocas(llvm::Function *func,
                                         const std::vector<std::string> &args) {
    llvm::Function::arg_iterator iterator = func->arg_begin();
    for ( auto &arg : args ) {
      llvm::Argument *val = &(*iterator++);
      llvm::AllocaInst *alloca = create_entry_block_alloca(func, arg);
      builder->CreateStore(val, alloca);
      set_named_value(arg, alloca);
    }
}
