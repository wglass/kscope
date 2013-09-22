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
    virtual ~ASTNode() {}
    virtual Value *codegen() = 0;
};

class NumberNode : public ASTNode {
    double val;
public:
    NumberNode(double val) : val(val) {}
    virtual Value *codegen();
};

class VariableNode : public ASTNode {
    std::string name;
public:
    VariableNode(const std::string &name) : name(name) {}
    const std::string &getName() const { return name; }

    virtual Value *codegen();
};

class BinaryNode : public ASTNode {
    char op;
    ASTNode *lhs, *rhs;
public:
    BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
        : op(op), lhs(lhs), rhs(rhs) {}
    virtual Value *codegen();
};

class UnaryNode : public ASTNode {
    char opcode;
    ASTNode *operand;

public:
    UnaryNode(char opcode, ASTNode *operand)
        : opcode(opcode), operand(operand) {}
    virtual Value *codegen();
};

class CallNode : public ASTNode {
    std::string callee;
    std::vector<ASTNode*> args;
public:
    CallNode(const std::string &callee, std::vector<ASTNode*> &args)
        : callee(callee), args(args) {}
    virtual Value *codegen();
};

class PrototypeNode : public ASTNode {
    std::string name;
    std::vector<std::string> args;
    bool is_operator;
    unsigned precedence;

public:
    PrototypeNode(
                  const std::string &name, const std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0
                  )
        : name(name), args(args), is_operator(is_operator), precedence(precedence) {}

    bool isUnaryOp() const {
        return is_operator && args.size() == 1;
    }
    bool isBinaryOp() const {
        return is_operator && args.size() == 2;
    }

    char getOperatorName() const {
        assert(isUnaryOp() || isBinaryOp());
        return name[name.size() - 1];
    }

    unsigned getBinaryPrecedence() const {
        return precedence;
    }

    Function *codegen();
    void CreateArgumentAllocas(Function *func);
};

class FunctionNode : public ASTNode {
    PrototypeNode *proto;
    ASTNode *body;
public:
    FunctionNode(PrototypeNode *proto, ASTNode *body)
        : proto(proto), body(body) {}
    Function *codegen();
};

class VarNode : public ASTNode {
    std::vector<std::pair<std::string, ASTNode*> > var_names;
    ASTNode *body;
public:
    VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names, ASTNode *body)
        : var_names(var_names), body(body) {}
    virtual Value *codegen();
};

class IfNode : public ASTNode {
    ASTNode *condition, *then, *_else;
public:
    IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
        : condition(cond), then(then), _else(_else) {};
    virtual Value *codegen();
};

class ForNode: public ASTNode {
    std::string var_name;
    ASTNode *start, *end, *step, *body;
public:
    ForNode(const std::string &var_name,
            ASTNode *start, ASTNode *end, ASTNode *step, ASTNode *body)
        : var_name(var_name), start(start), end(end), step(step), body(body) {};
    virtual Value *codegen();
};


#endif
