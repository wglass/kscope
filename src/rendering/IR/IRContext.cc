#include "IRContext.h"

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

IRContext::IRContext()
  : llvm_context(llvm::getGlobalContext()),
    module(std::make_unique<llvm::Module>(generate_module_name(), llvm_context)) {}


IRContext::IRContext(llvm::LLVMContext &llvm_context,
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

IRContext::IRContext(IRContext &&other)
  : llvm_context(other.llvm_context) {
  module = std::move(other.module);
  builder = std::move(other.builder);
  pass_manager = std::move(other.pass_manager);
}

IRContext::~IRContext() {
  module.release();
  builder.release();
  pass_manager.release();
}

llvm::LLVMContext &
IRContext::get_llvm_context() {
  return llvm_context;
}

llvm::Module &
IRContext::get_module() {
  return *module;
}

llvm::IRBuilder<> &
IRContext::get_builder() {
  return *builder;
}

llvm::legacy::FunctionPassManager &
IRContext::get_pass_manager() {
  return *pass_manager;
}

bool
IRContext::has_module() {
  if ( ! module ) {
    return false;
  } else {
    return true;
  }
}

std::unique_ptr<llvm::Module>
IRContext::give_up_module() {
  std::unique_ptr<llvm::Module> mod = std::move(module);

  builder.release();
  pass_manager.release();

  return mod;
}

llvm::AllocaInst *
IRContext::get_named_value (const std::string &name){
    return named_values[name];
}

void
IRContext::set_named_value(const std::string &name,
                           llvm::AllocaInst *value) {
    named_values[name] = value;
}

void
IRContext::clear_named_value(const std::string &name) {
    named_values.erase(name);
}

void
IRContext::clear_all_named_values() {
    named_values.clear();
}

llvm::AllocaInst *
IRContext::create_entry_block_alloca(llvm::Function *func,
                                     const std::string &name) {
  llvm::IRBuilder<> tmp_builder(&func->getEntryBlock(),
                                func->getEntryBlock().begin());

  return tmp_builder.CreateAlloca(llvm::Type::getDoubleTy(llvm_context),
                                  0,
                                  name.c_str());
}

void
IRContext::create_argument_allocas(llvm::Function *func,
                                   const std::vector<std::string> &args) {
    llvm::Function::arg_iterator iterator = func->arg_begin();
    for ( auto &arg : args ) {
      llvm::Argument *val = &(*iterator++);
      llvm::AllocaInst *alloca = create_entry_block_alloca(func, arg);
      builder->CreateStore(val, alloca);
      set_named_value(arg, alloca);
    }
}
