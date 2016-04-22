#pragma once

#include "rendering/RenderSpec.h"

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Value.h"


class IRRenderSpec : public RenderSpec {
public:
  typedef llvm::Value Result;
  typedef llvm::orc::TargetAddress FuncRepresentation;
};
