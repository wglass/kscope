#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/TargetSelect.h"

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "ast/function.h"
#include "ast/prototype.h"
#include "codegen/renderer.h"
#include "parser.h"

using ::llvm::Function;
using ::llvm::InitializeNativeTarget;


static void
HandleDefinition(IRRenderer *renderer, Parser *parser) {
    if ( FunctionNode *node = parser->parse_definition() ) {
        if ( Function *func = node->codegen(renderer) ) {
            fprintf(stderr, "Read function definition:");
            func->dump();
        }
    } else {
        parser->lexer->next();
    }
}

static void
HandleExtern(IRRenderer *renderer, Parser *parser) {
    if ( PrototypeNode *node = parser->parse_extern() ) {
        if ( Function *func = node->codegen(renderer) ) {
            fprintf(stderr, "Read extern:");
            func->dump();
        }
    } else {
        parser->lexer->next();
    }
}

static void
HandleTopLevelExpression(IRRenderer *renderer, Parser *parser) {
    if ( FunctionNode *node = parser->parse_top_level_expression() ) {
        if ( Function *func = node->codegen(renderer) ) {
            void *func_ptr = renderer->engine->getPointerToFunction(func);

            double (*func_pointer)() = (double (*)())(intptr_t)func_ptr;

            fprintf(stderr, "Evaluated to: %f\n", func_pointer());
        }
    } else {
        parser->lexer->next();
    }
}

static void MainLoop(IRRenderer *renderer, Parser *parser) {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (parser->lexer->current_token()) {
        case (int)Token::_EOF:
            renderer->module->dump();
            return;
        case ';':
            parser->lexer->next();
            break;
        case (int)Token::DEF:
            HandleDefinition(renderer, parser);
            break;
        case (int)Token::EXTERN:
            HandleExtern(renderer, parser);
            break;
        default:
            HandleTopLevelExpression(renderer, parser);
            break;
        }
    }
}

int main() {
    InitializeNativeTarget();

    IRRenderer *renderer = new IRRenderer();
    Parser *parser = new Parser();

    fprintf(stderr, "ready> ");

    parser->lexer->next();

    MainLoop(renderer, parser);

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
