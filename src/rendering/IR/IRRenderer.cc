#include "IRRenderer.h"

#include "ast/ASTNode.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Scalar.h"

#include <string>


using ::llvm::AllocaInst;
using ::llvm::CloneModule;
using ::llvm::DataLayout;
using ::llvm::ExecutionEngine;
using ::llvm::EngineBuilder;
using ::llvm::Function;
using ::llvm::legacy::FunctionPassManager;
using ::llvm::IRBuilder;
using ::llvm::LLVMContext;
using ::llvm::Module;
using ::llvm::Type;


IRRenderer::IRRenderer(std::unique_ptr<ORCPipeline> pipeline)
  : pipeline(pipeline),
    llvm_context(llvm::getGlobalContext()),
    target_machine(llvm::EngineBuilder().selectTarget()) {}

IRRenderer::IRRenderer(const IRRenderer &other)
    : IRRenderer(CloneModule(other.module.get())) {}

IRRenderer::IRRenderer(IRRenderer &&other) {
  pipeine = std::move(other.pipeline);
  render_context = std::move(other.render_context);
  target_machine = std::move(other.target_machine);
  llvm_context = other.llvm_context;
}

IRRenderer &
IRRenderer::operator =(IRRenderer other) {
    std::swap(pipeline, other.pipeline);
    std::swap(render_context, other.render_context);
    std::swap(target_machine, other.target_machine);
    return *this;
}

IRRenderer::~IRRenderer() {
  pipeline.release();
  render_context.release();
}

llvm::Value *
IRRenderer::render(ASTNode *node) {
  return node->render<llvm::Value>(this);
}

llvm::AllocaInst *
IRRenderer::get_named_value (const std::string &name){
    return named_values[name];
}

void
IRRenderer::set_named_value(const std::string &name, llvm::AllocaInst *value) {
    named_values[name] = value;
}

void
IRRenderer::clear_named_value(const std::string &name) {
    named_values.erase(name);
}

void
IRRenderer::clear_all_named_values() {
    named_values.clear();
}

AllocaInst *
IRRenderer::create_entry_block_alloca(Function *func, const std::string &name) {
    IRBuilder<> tmp_builder(&func->getEntryBlock(),
                            func->getEntryBlock().begin());

    return tmp_builder.CreateAlloca(Type::getDoubleTy(llvm::getGlobalContext()),
                                    0,
                                    name.c_str());
}


void
IRRenderer::create_argument_allocas(Function *func, const std::vector<std::string> &args) {
    Function::arg_iterator iterator = func->arg_begin();
    for ( auto &arg : args ) {
      llvm::Argument *val = &(*iterator++);
      AllocaInst *alloca = create_entry_block_alloca(func, arg);
      builder->CreateStore(val, alloca);
      set_named_value(arg, alloca);
    }
}
