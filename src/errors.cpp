#include <cstdio>

#include "errors.h"

ASTNode *Error(const char *message) {
    fprintf(stderr, "Error, %s\n", message);
    return 0;
}
PrototypeNode *ErrorP(const char *message) {
    Error(message);
    return 0;
}
FunctionNode *ErrorF(const char *message) {
    Error(message);
    return 0;
}
Value *ErrorV(const char *message) {
    Error(message);
    return 0;
}
