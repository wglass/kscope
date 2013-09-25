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
#include "codegen/context.h"
#include "parser.h"

using ::llvm::Function;
using ::llvm::InitializeNativeTarget;


static void
HandleDefinition(Context *context, Parser *parser) {
    if ( FunctionNode *node = parser->parse_definition() ) {
        if ( Function *func = node->codegen(context) ) {
            fprintf(stderr, "Read function definition:");
            func->dump();
        }
    } else {
        parser->lexer()->next();
    }
}

static void
HandleExtern(Context *context, Parser *parser) {
    if ( PrototypeNode *node = parser->parse_extern() ) {
        if ( Function *func = node->codegen(context) ) {
            fprintf(stderr, "Read extern:");
            func->dump();
        }
    } else {
        parser->lexer()->next();
    }
}

static void
HandleTopLevelExpression(Context *context, Parser *parser) {
    if ( FunctionNode *node = parser->parse_top_level_expression() ) {
        if ( Function *func = node->codegen(context) ) {
            void *func_ptr = context->engine()->getPointerToFunction(func);

            double (*func_pointer)() = (double (*)())(intptr_t)func_ptr;

            fprintf(stderr, "Evaluated to: %f\n", func_pointer());
        }
    } else {
        parser->lexer()->next();
    }
}

static void MainLoop(Context *context, Parser *parser) {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (parser->lexer()->current_token()) {
        case Token::_EOF:
            context->module()->dump();
            return;
        case ';':
            parser->lexer()->next();
            break;
        case Token::DEF:
            HandleDefinition(context, parser);
            break;
        case Token::EXTERN:
            HandleExtern(context, parser);
            break;
        default:
            HandleTopLevelExpression(context, parser);
            break;
        }
    }
}

int main() {
    InitializeNativeTarget();

    Context *context = new Context();
    Parser *parser = new Parser();

    parser->lexer()->add_op_precedence('=', 2);
    parser->lexer()->add_op_precedence('<', 10);
    parser->lexer()->add_op_precedence('+', 20);
    parser->lexer()->add_op_precedence('-', 20);
    parser->lexer()->add_op_precedence('*', 240);

    fprintf(stderr, "ready> ");

    parser->lexer()->next();

    MainLoop(context, parser);

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
