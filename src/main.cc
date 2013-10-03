#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"

#include <iostream>
#include <sstream>
#include <string>

#include "codegen/renderer.h"
#include "parsing/tree.h"


int main() {
    llvm::InitializeNativeTarget();

    IRRenderer *renderer = new IRRenderer();

    STree *tree = new STree();

    std::string input;
    fprintf(stderr, "ready> ");
    while (std::getline(std::cin, input, ';')) {
        std::istringstream iss(input);

        tree->parse(iss);
        if ( tree->root != 0 ) {
            if ( Function *func = static_cast<Function*>(tree->root->codegen(renderer)) ) {
                if ( func->getName() == "" ) {
                    void *func_ptr = renderer->engine->getPointerToFunction(func);
                    double (*func_pointer)() = (double(*)())(intptr_t)func_ptr;
                    fprintf(stderr, "Evaluated to: %f\n", func_pointer());
                }
            }
        }
        fprintf(stderr, "ready> ");
    }

    renderer->module->dump();

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
