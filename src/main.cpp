#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>


#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

using namespace llvm;

static void HandleDefinition() {
  if ( FunctionNode *node = ParseDefinition() ) {
      if ( Function *func = node->codegen() ) {
          fprintf(stderr, "Read function definition:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void HandleExtern() {
  if ( PrototypeNode *node = ParseExtern() ) {
      if ( Function *func = node->codegen() ) {
          fprintf(stderr, "Read extern:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if ( FunctionNode *node = ParseTopLevelExpression() ) {
      if ( Function *func = node->codegen() ) {
          void *func_ptr = engine->getPointerToFunction(func);

          double (*func_pointer)() = (double (*)())(intptr_t)func_ptr;

          fprintf(stderr, "Evaluated to: %f\n", func_pointer());
      }
  } else {
    getNextToken();
  }
}

static void MainLoop() {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (current_token) {
        case tok_eof: return;
        case ';':
            getNextToken();
            break;
        case tok_def:
            HandleDefinition();
            break;
        case tok_extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

int main() {
    InitializeNativeTarget();

    LLVMContext &context = getGlobalContext();

    op_precedence['='] = 2;
    op_precedence['<'] = 10;
    op_precedence['+'] = 20;
    op_precedence['-'] = 20;
    op_precedence['*'] = 40;

    fprintf(stderr, "ready> ");
    getNextToken();

    module = new Module("my cool jit", context);

    std::string error_string;
    engine = EngineBuilder(module).setErrorStr(&error_string).create();
    if ( ! engine ) {
        fprintf(
                stderr,
                "Could not create execution engine: %s\n",
                error_string.c_str()
                );
        exit(1);
    }

    FunctionPassManager pass_manager(module);

    pass_manager.add(new DataLayout(*engine->getDataLayout()));
    pass_manager.add(createBasicAliasAnalysisPass());
    pass_manager.add(createPromoteMemoryToRegisterPass());
    pass_manager.add(createInstructionCombiningPass());
    pass_manager.add(createReassociatePass());
    pass_manager.add(createGVNPass());
    pass_manager.add(createCFGSimplificationPass());

    pass_manager.doInitialization();

    func_pass_manager = &pass_manager;

    MainLoop();

    func_pass_manager = 0;

    module->dump();

    return 0;
}

/// putchard - putchar that takes a double and returns 0.
extern "C"
double putchard(double X) {
  putchar((char)X);
  return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C"
double printd(double X) {
  printf("%f\n", X);
  return 0;
}
