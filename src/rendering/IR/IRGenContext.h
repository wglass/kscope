#pragma once

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "SessionContext.h"


class IRGenContext {
public:

  IRGenContext(SessionContext &session);

  SessionContext& getSession() { return session; }
  llvm::Module& getM() const { return *module; }
  std::unique_ptr<Module> take_module() { return std::move(module); }
  llvm::IRBuilder<>& getBuilder() { return builder; }
  llvm::LLVMContext& getLLVMContext() { return session.getLLVMContext(); }
  llvm::Function* getPrototype(const std::string &name);

  std::map<std::string, llvm::AllocaInst*> NamedValues;

private:
  SessionContext &session;
  std::unique_ptr<llvm::Module> module;
  llvm::IRBuilder<> builder;
};
