#include "kscope/AST/ASTNodes.h"
#include "kscope/AST/ASTree.h"

#include "Render/IR/IRRenderer.h"
#include "Render/IR/Pipeline/SimpleORCPipeline.h"
#include "Render/IR/Pipeline/LazyORCPipeline.h"

#include "kscope/Interop/Printing.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"

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
  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  auto pipeline_choice = lazy ? PipelineChoice::Lazy : PipelineChoice::Simple;

  auto renderer = std::make_unique<IRRenderer>(pipeline_choice);

  auto tree = std::make_shared<ASTree>();

  std::string input;
  fprintf(stderr, "kscope> ");
  while (std::getline(std::cin, input, ';')) {
    std::istringstream iss(input);

    tree->parse(iss);
    if ( ! tree->root ) {
      fprintf(stderr, "kscope> ");
      continue;
    }

    renderer->visit(tree->root.get());

    if ( tree->root->kind == ASTNodeKind::Function ) {
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
