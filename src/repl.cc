#include "ast/FunctionNode.h"
#include "parsing/ASTree.h"
#include "rendering/IR/IRRenderer.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/Function.h"

#include <iostream>
#include <sstream>
#include <string>


int main() {
    llvm::InitializeNativeTarget();

    IRRenderer *renderer = new IRRenderer();

    std::shared_ptr<ASTree> tree = std::make_shared<ASTree>();

    std::string input;
    fprintf(stderr, "kscope> ");
    while (std::getline(std::cin, input, ';')) {
        std::istringstream iss(input);

        tree->parse(iss);
        if ( tree->root != 0 ) {
          renderer->render_tree(tree);

          if ( FunctionNode *func_node = static_cast<FunctionNode*>(tree->root.get()) ) {
            fprintf(stderr, "Getting function %s\n", func_node->proto->name.c_str());
            auto func_ptr = renderer->get_function(func_node->proto->name);
            double (*func_pointer)() = (double(*)())(intptr_t)func_ptr;
            if ( func_pointer ) {
              fprintf(stderr, "Evaluated to: %f\n", func_pointer());
            } else {
              fprintf(stderr, "Pointer was null!\n");
            }
          } else {
            fprintf(stderr, "Cast to function node didn't work\n");
          }
        }

        fprintf(stderr, "kscope> ");
    }

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
