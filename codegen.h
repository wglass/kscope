#ifndef __CODEGEN_H_INCLUDED__
#define __CODEGEN_H_INCLUDED__

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"

#include <map>
#include <string>

using namespace llvm;

extern Module *module;
extern std::map<std::string, AllocaInst*> named_values;
extern FunctionPassManager *func_pass_manager;
extern ExecutionEngine *engine;


#endif
