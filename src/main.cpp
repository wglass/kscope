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


#include "ast/function.h"
#include "ast/prototype.h"
#include "lexer.h"
#include "parser.h"
#include "codegen/context.h"


using namespace llvm;

static void
HandleDefinition(Context *context) {
  if ( FunctionNode *node = ParseDefinition() ) {
      if ( Function *func = node->codegen(context) ) {
          fprintf(stderr, "Read function definition:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void
HandleExtern(Context *context) {
  if ( PrototypeNode *node = ParseExtern() ) {
      if ( Function *func = node->codegen(context) ) {
          fprintf(stderr, "Read extern:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void
HandleTopLevelExpression(Context *context) {
  if ( FunctionNode *node = ParseTopLevelExpression() ) {
      if ( Function *func = node->codegen(context) ) {
void *func_ptr = context->engine()->getPointerToFunction(func);

          double (*func_pointer)() = (double (*)())(intptr_t)func_ptr;

          fprintf(stderr, "Evaluated to: %f\n", func_pointer());
      }
  } else {
    getNextToken();
  }
}

static void
MainLoop(Context *context) {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (current_token) {
        case tok_eof: return;
        case ';':
            getNextToken();
            break;
        case tok_def:
            HandleDefinition(context);
            break;
        case tok_extern:
            HandleExtern(context);
            break;
        default:
            HandleTopLevelExpression(context);
            break;
        }
    }

context->module()->dump();
}

int main() {
InitializeNativeTarget();

Context *context = new Context();

    op_precedence['='] = 2;
    op_precedence['<'] = 10;
    op_precedence['+'] = 20;
    op_precedence['-'] = 20;
    op_precedence['*'] = 40;

    fprintf(stderr, "ready> ");
    getNextToken();

    MainLoop(context);

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
