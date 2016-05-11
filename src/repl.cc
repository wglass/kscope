#include "kscope/AST/ASTree.h"
#include "kscope/AST/FunctionNode.h"

#include "Render/IR/IRRenderer.h"
#include "Render/IR/Pipeline/SimpleORCPipeline.h"
#include "Render/IR/Pipeline/LazyORCPipeline.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CommandLine.h"

#include <iostream>
#include <sstream>
#include <string>


int main(int argc, char** argv) {
  llvm::cl::OptionCategory kscope_options("kscope options",
                                          "Options for the kscope compiler.");

  llvm::cl::opt<bool> lazy("lazy",
                           llvm::cl::desc("Use the lazy compiling pipeline."),
                           llvm::cl::cat(kscope_options));

  llvm::cl::HideUnrelatedOptions(kscope_options);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto pipeline_choice = lazy ? PipelineChoice::Lazy : PipelineChoice::Simple;

  auto *renderer = new IRRenderer(pipeline_choice);

  std::shared_ptr<ASTree> tree = std::make_shared<ASTree>();

  std::string input;
  fprintf(stderr, "kscope> ");
  while (std::getline(std::cin, input, ';')) {
    std::istringstream iss(input);

    tree->parse(iss);
    if ( tree->root != 0 ) {
      renderer->render_tree(tree);

      FunctionNode *func_node = static_cast<FunctionNode*>(tree->root.get());

      if ( func_node == nullptr ) {
        fprintf(stderr, "kscope> ");
        continue;
      }

      if ( func_node->proto->is_anon ) {
        auto func_ptr = renderer->get_symbol(func_node->proto->name);
        double (*func_pointer)() = (double(*)())(intptr_t)func_ptr;
        fprintf(stderr, "Evaluated to: %f\n", func_pointer());
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
