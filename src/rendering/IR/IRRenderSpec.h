#pragma once

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Value.h"


class IRRenderSpec {
public:
  typedef llvm::Value Result;
  typedef llvm::Function TopLevelResult;
  typedef llvm::orc::TargetAddress FuncRepr;
};
