#ifndef __AST_H_INCLUDED__
#define __AST_H_INCLUDED__

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"


using namespace llvm;

#include <string>
#include <vector>

class ASTNode {
public:
    virtual ~ASTNode() {};
    virtual Value *codegen() = 0;
};

class NumberNode : public ASTNode {

    double val;

public:

    NumberNode(double val);
    virtual Value *codegen();
};

class VariableNode : public ASTNode {

    std::string name;

public:
    VariableNode(const std::string &name);
    const std::string getName() const;

    virtual Value *codegen();
};

class BinaryNode : public ASTNode {
    char op;
    ASTNode *lhs, *rhs;

public:
    BinaryNode(char op, ASTNode *lhs, ASTNode *rhs);
    virtual Value *codegen();
};

class UnaryNode : public ASTNode {
    char opcode;
    ASTNode *operand;

public:
    UnaryNode(char opcode, ASTNode *operand);
    virtual Value *codegen();
};

class CallNode : public ASTNode {
    std::string callee;
    std::vector<ASTNode*> args;

public:
    CallNode(const std::string &callee, std::vector<ASTNode*> &args);
    virtual Value *codegen();
};

class PrototypeNode : public ASTNode {
    std::string name;
    std::vector<std::string> args;
    bool is_operator;
    unsigned precedence;

public:
    PrototypeNode(const std::string &name,
                  const std::vector<std::string> &args,
                  bool is_operator = false,
                  unsigned precedence = 0);

    bool isUnaryOp() const;
    bool isBinaryOp() const;

    char getOperatorName() const;
    unsigned getBinaryPrecedence() const;

    Function *codegen();
    void CreateArgumentAllocas(Function *func);
};

class FunctionNode : public ASTNode {
    PrototypeNode *proto;
    ASTNode *body;

public:
    FunctionNode(PrototypeNode *proto, ASTNode *body);
    Function *codegen();
};

class VarNode : public ASTNode {
    std::vector<std::pair<std::string, ASTNode*> > var_names;
    ASTNode *body;

public:
    VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
            ASTNode *body);
    virtual Value *codegen();
};

class IfNode : public ASTNode {
    ASTNode *condition, *then, *_else;

public:
    IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else);
    virtual Value *codegen();
};

class ForNode: public ASTNode {
    std::string var_name;
    ASTNode *start, *end, *step, *body;

public:
    ForNode(const std::string &var_name,
            ASTNode *start, ASTNode *end, ASTNode *step,
            ASTNode *body);
    virtual Value *codegen();
};


#endif
