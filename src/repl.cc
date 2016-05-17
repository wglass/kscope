#include "kscope/AST/ASTNodes.h"
#include "kscope/AST/ASTree.h"

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

  auto renderer = std::make_unique<IRRenderer>(pipeline_choice);

  auto tree = std::make_shared<ASTree>();

  std::string input;
  fprintf(stderr, "kscope> ");
  while (std::getline(std::cin, input, ';')) {
    std::istringstream iss(input);

    tree->parse(iss);
    if ( ! tree->root ) {
      continue;
    }

    renderer->visit(tree->root.get());

    if ( tree->root->kind == ASTNodeKind::Prototype ) {
      auto *proto_node = static_cast<PrototypeNode*>(tree->root.get());
      fprintf(stderr, "Added new extern: %s\n", proto_node->name.c_str());
    } else if ( tree->root->kind == ASTNodeKind::Function ) {
      auto *func_node = static_cast<FunctionNode*>(tree->root.get());

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
