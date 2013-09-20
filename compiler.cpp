#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/DataLayout.h"
#include "llvm/DerivedTypes.h"
#include "llvm/IRBuilder.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"


using namespace llvm;


enum Token {
    tok_eof = -1,
    tok_def = -2, tok_extern = -3,
    tok_identifier = -4, tok_number = -5,
    tok_if = -6, tok_then = -7, tok_else = -8,
    tok_for = -9, tok_in = -10,
    tok_binary = -11, tok_unary = -12,
    tok_var = -13
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
    static int last_char = ' ';

    while ( isspace(last_char) ) {
        last_char = getchar();
    }

    if ( isalpha(last_char) ) {
        IdentifierStr = last_char;
        while ( isalnum((last_char = getchar())) ) {
            IdentifierStr += last_char;
        }

        if ( IdentifierStr == "def" ) {
            return tok_def;
        }
        if ( IdentifierStr == "extern" ) {
            return tok_extern;
        }
        if ( IdentifierStr == "if" ) {
            return tok_if;
        }
        if ( IdentifierStr == "then" ) {
            return tok_then;
        }
        if ( IdentifierStr == "else" ) {
            return tok_else;
        }
        if ( IdentifierStr == "for" ) {
            return tok_for;
        }
        if ( IdentifierStr == "in" ) {
            return tok_in;
        }
        if ( IdentifierStr == "binary" ) {
            return tok_binary;
        }
        if ( IdentifierStr == "unary" ) {
            return tok_unary;
        }
        if ( IdentifierStr == "var" ) {
            return tok_var;
        }

        return tok_identifier;
    }

    if ( isdigit(last_char) || last_char == '.' ) {
        std::string number_string;

        do {
            number_string += last_char;
            last_char = getchar();
        } while ( isdigit(last_char) || last_char == '.' );

        NumVal = strtod(number_string.c_str(), 0);
        return tok_number;
    }

    if ( last_char == '#' ) {
        do {
            last_char = getchar();
        } while ( last_char != EOF && last_char != '\n' && last_char != '\r' );

        if ( last_char != EOF ) {
            return gettok();
        }
    }

    if ( last_char == EOF ) {
        return tok_eof;
    }

    int this_char = last_char;
    last_char = getchar();

    return this_char;
}

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

static int current_token;
static int getNextToken() {
    return current_token = gettok();
}

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

static ASTNode *ParseExpression();
static ASTNode *ParseIf();
static ASTNode *ParseFor();
static ASTNode *ParseUnary();

static ASTNode *ParseNumber() {
    ASTNode *Result = new NumberNode(NumVal);
    getNextToken();
    return Result;
}

static ASTNode *ParseParen() {
    getNextToken();
    ASTNode *V = ParseExpression();
    if ( !V ) {
        return 0;
    }

    if ( current_token != ')' ) {
        return Error("expected ')'");
    }
    getNextToken();
    return V;
}

static ASTNode *ParseIdentifier() {
    std::string IdName = IdentifierStr;

    getNextToken();

    if ( current_token != '(' ) {
        return new VariableNode(IdName);
    }

    getNextToken();
    std::vector<ASTNode*> Args;
    if ( current_token != ')' ) {
        while (1) {
            ASTNode *Arg = ParseExpression();
            if ( !Arg ) {
                return 0;
            }

            Args.push_back(Arg);

            if ( current_token == ')') {
                break;
            }

            if ( current_token != ',' ) {
                return Error("expected ')' or ',' in argument list");
            }

            getNextToken();
        }
    }
    getNextToken();

    return new CallNode(IdName, Args);
}

static ASTNode *ParseVar() {
    getNextToken();

    std::vector<std::pair<std::string, ASTNode*> > var_names;

    if ( current_token != tok_identifier ) {
        return Error("Expected identifier after var");
    }

    while (1) {
        std::string name = IdentifierStr;
        getNextToken();

        ASTNode *init = 0;
        if ( current_token == '=' ) {
            getNextToken();
            init = ParseExpression();
            if ( init == 0 ) { return 0; }
        }

        var_names.push_back(std::make_pair(name, init));

        if ( current_token != ',') { break; }

        getNextToken();

        if ( current_token != tok_identifier ) {
            return Error("Expected identifier list after var");
        }
    }

    if ( current_token != tok_in ) {
        return Error("Expected 'in' keyword after 'var'");
    }
    getNextToken();

    ASTNode *body = ParseExpression();
    if ( body == 0 ) { return 0; }

    return new VarNode(var_names, body);
}

static ASTNode *ParsePrimary() {
    switch( current_token ) {
    default:
        return Error("unknown token when expecting an expression");
    case tok_identifier: return ParseIdentifier();
    case tok_number: return ParseNumber();
    case '(': return ParseParen();
    case tok_if: return ParseIf();
    case tok_for: return ParseFor();
    case tok_var: return ParseVar();
    }
}


static std::map<char, int> op_precedence;

static int GetTokPrecedence() {
    if ( !isascii(current_token) ) {
        return -1;
    }

    int tokPrec = op_precedence[current_token];
    if ( tokPrec <= 0 ) {
        return -1;
    }
    return tokPrec;
}


static ASTNode *ParseBinOpRHS(int expression_precedence, ASTNode *LHS) {
    while (1) {
        int token_precedence = GetTokPrecedence();
        if ( token_precedence < expression_precedence ) {
            return LHS;
        }

        int BinOp = current_token;
        getNextToken();

        ASTNode *RHS = ParseUnary();
        if ( !RHS ) { return 0; }

        int next_precedence = GetTokPrecedence();
        if ( token_precedence < next_precedence ) {
            RHS = ParseBinOpRHS(token_precedence + 1, RHS);
        }

        LHS = new BinaryNode(BinOp, LHS, RHS);
    }
}

static ASTNode *ParseUnary() {
    if ( !isascii(current_token) || current_token == '(' || current_token == ')' ) {
        return ParsePrimary();
    }

    int opr = current_token;
    getNextToken();
    if ( ASTNode *operand = ParseUnary() ) {
        return new UnaryNode(opr, operand);
    }

    return 0;
}

static ASTNode *ParseExpression() {
    ASTNode *LHS = ParseUnary();
    if ( !LHS ) { return 0; }

    return ParseBinOpRHS(0, LHS);
}

static PrototypeNode *ParsePrototype() {
    std::string func_name;

    unsigned kind = 0;
    unsigned binary_precedence = 0;

    switch(current_token) {
    case tok_identifier:
        kind = 0;
        func_name = IdentifierStr;
        getNextToken();
        break;
    case tok_unary:
        getNextToken();
        if ( ! isascii(current_token) ) {
            return ErrorP("Expected unary operator");
        }
        kind = 1;
        func_name = "unary";
        func_name += (char)current_token;
        getNextToken();
        break;
    case tok_binary:
        getNextToken();
        if ( ! isascii(current_token) ) {
            return ErrorP("Expected binary operator");
        }
        kind = 2;
        func_name = "binary";
        func_name += (char)current_token;
        getNextToken();
        if ( current_token == tok_number ) {
            if ( NumVal < 1 || NumVal > 100 ) {
                return ErrorP("Invalid precedence: must be 1..100");
            }
            binary_precedence = (unsigned)NumVal;
            getNextToken();
        }
        break;
    }

    if ( current_token != '(' ) {
        return ErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> arg_names;

    while ( getNextToken() == tok_identifier ) {
        arg_names.push_back(IdentifierStr);
    }

    if ( current_token != ')' ) {
        return ErrorP("Expected ')' in prototype");
    }

    getNextToken();

    if ( kind && arg_names.size() != kind ) {
        return ErrorP("Invalid number of operands for operator");
    }

    return new PrototypeNode(func_name, arg_names, kind != 0, binary_precedence);
}

static FunctionNode *ParseDefinition() {
    getNextToken();

    PrototypeNode *Proto = ParsePrototype();

    if ( Proto == 0 ) { return 0; }

    if ( ASTNode *E = ParseExpression() ) {
        return new FunctionNode(Proto, E);
    }

    return 0;
}

static PrototypeNode *ParseExtern() {
    getNextToken();

    return ParsePrototype();
}

static FunctionNode *ParseTopLevelExpression() {
    if ( ASTNode *E = ParseExpression() ) {
        PrototypeNode *Proto = new PrototypeNode("", std::vector<std::string>());
        return new FunctionNode(Proto, E);
    }

    return 0;
}

static ASTNode *ParseIf() {
    getNextToken();

    ASTNode *condition = ParseExpression();
    if ( ! condition ) { return 0; }

    if ( current_token != tok_then ) {
        return Error("expected 'then'");
    }

    getNextToken();

    ASTNode *then = ParseExpression();
    if ( ! then ) { return 0; }

    if ( current_token != tok_else ) {
        return Error("expected 'else'");
    }

    getNextToken();

    ASTNode *_else = ParseExpression();
    if ( ! _else ) { return 0; }

    return new IfNode(condition, then, _else);
}

static ASTNode *ParseFor() {
    getNextToken();

    if ( current_token != tok_identifier ) {
        return Error("expected identifier after 'for'");
    }

    std::string id_name = IdentifierStr;
    getNextToken();

    if ( current_token != '=' ) {
        return Error("expected '=' after 'for'");
    }
    getNextToken();

    ASTNode *start = ParseExpression();
    if ( start == 0 ) { return 0; }

    if ( current_token != ',' ) {
        return Error("Expected ',' after for start value");
    }
    getNextToken();

    ASTNode *end = ParseExpression();
    if ( end == 0 ) { return 0; }

    ASTNode *step = 0;
    if ( current_token == ',' ) {
        getNextToken();
        step = ParseExpression();
        if ( step == 0 ) { return 0; }
    }

    if ( current_token != tok_in ) {
        return Error("Expected 'in' after 'for'");
    }
    getNextToken();

    ASTNode *body = ParseExpression();
    if ( body == 0 ) { return 0; }

    return new ForNode(id_name, start, end, step, body);
}


static Module *module;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, AllocaInst*> named_values;
static FunctionPassManager *func_pass_manager;
static ExecutionEngine *engine;


static AllocaInst *CreateEntryBlockAlloca(Function *func, const std::string &var_name) {
    IRBuilder<> TmpB(&func->getEntryBlock(), func->getEntryBlock().begin());

    return TmpB.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0, var_name.c_str());
}

Value *NumberNode::codegen() {
    return ConstantFP::get(getGlobalContext(), APFloat(val));
}

Value *VariableNode::codegen() {
    Value *val = named_values[name];
    if ( !val ) {
        return ErrorV("Unknown variable name");
    }

    return Builder.CreateLoad(val, name.c_str());
}

Value *BinaryNode::codegen() {
    if ( op == '=' ) {
        VariableNode *lhse = dynamic_cast<VariableNode*>(lhs);
        if ( ! lhse ) {
            return ErrorV("destination of '=' must be a variable");
        }

        Value *val = rhs->codegen();
        if ( val == 0 ) { return 0; }

        Value *variable = named_values[lhse->getName()];
        if ( variable == 0 ) {
            return ErrorV("Unknown variable name");
        }

        Builder.CreateStore(val, variable);

        return val;
    }

    Value *left = lhs->codegen();
    Value *right = rhs->codegen();

    if (left == 0 || right == 0 ) { return 0; }

    switch (op) {
    case '+': return Builder.CreateFAdd(left, right, "addtmp");
    case '-': return Builder.CreateFSub(left, right, "subtmp");
    case '*': return Builder.CreateFMul(left, right, "multmp");
    case '<':
        left = Builder.CreateFCmpULT(left, right, "cmptmp");
        return Builder.CreateUIToFP(left, Type::getDoubleTy(getGlobalContext()), "booltmp");
    default: break;
    }

    Function *func = module->getFunction(std::string("binary") + op);
    assert(func && "binary operator not found!");

    Value *operands[2] = { left, right };

    return Builder.CreateCall(func, operands, "binop");
}

Value *UnaryNode::codegen() {
    Value *operand_value = operand->codegen();
    if ( operand_value == 0 ) { return 0; }

    Function *func = module->getFunction(std::string("unary") + opcode);
    if ( func == 0 ) {
        return ErrorV("Unknown unary operator");
    }

    return Builder.CreateCall(func, operand_value, "unop");
}

Value *CallNode::codegen() {
    Function *callee_func = module->getFunction(callee);
    if ( callee_func == 0 ) {
        return ErrorV("Unknown function referenced");
    }

    if ( callee_func->arg_size() != args.size() ) {
        return ErrorV("Incorrect number of arguments passed");
    }

    std::vector<Value*> arg_values;
    for ( unsigned i = 0, num_args = args.size(); i < num_args; ++i ) {
        arg_values.push_back(args[i]->codegen());
        if ( arg_values.back() == 0 ) { return 0; }
    }

    return Builder.CreateCall(callee_func, arg_values, "calltmp");
}

Function *PrototypeNode::codegen() {
    std::vector<Type*> Doubles(args.size(),
                               Type::getDoubleTy(getGlobalContext()));
    FunctionType *func_type = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                                Doubles, false);

    Function *func = Function::Create(func_type, Function::ExternalLinkage, name, module);

    if ( func->getName() != name ) {
        func->eraseFromParent();
        func = module->getFunction(name);

        if ( !func->empty() ) {
            ErrorF("redefinition of function");
            return 0;
        }
        if ( func->arg_size() != args.size() ) {
            ErrorF("redefintion of function with different number of args");
            return 0;
        }
    }

    unsigned index = 0;
    for (
         Function::arg_iterator iterator = func->arg_begin();
         index != args.size();
         ++iterator, ++index
         ) {
        iterator->setName(args[index]);
    }

    return func;
}

void PrototypeNode::CreateArgumentAllocas(Function *func) {
    Function::arg_iterator iterator = func->arg_begin();

    for (unsigned i = 0, size = args.size(); i != size; ++i, ++iterator) {
        AllocaInst *alloca = CreateEntryBlockAlloca(func, args[i]);
        Builder.CreateStore(iterator, alloca);
        named_values[args[i]] = alloca;
    }

}

Function *FunctionNode::codegen() {
    named_values.clear();

    Function *func = proto->codegen();
    if ( func == 0 ) { return 0; }

    if ( proto->isBinaryOp() ) {
        op_precedence[proto->getOperatorName()] = proto->getBinaryPrecedence();
    }

    BasicBlock *block = BasicBlock::Create(getGlobalContext(), "entry", func);
    Builder.SetInsertPoint(block);

    proto->CreateArgumentAllocas(func);

    if ( Value *retval = body->codegen() ) {
        Builder.CreateRet(retval);
        verifyFunction(*func);

        func_pass_manager->run(*func);

        return func;
    }

    func->eraseFromParent();

    if ( proto->isBinaryOp() ) {
        op_precedence.erase(proto->getOperatorName());
    }

    return 0;
}

Value *VarNode::codegen() {
    std::vector<AllocaInst *> old_bindings;

    Function *func = Builder.GetInsertBlock()->getParent();

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        const std::string &var_name = var_names[i].first;
        ASTNode *init = var_names[i].second;

        Value *init_val;
        if ( init ) {
            init_val = init->codegen();
            if ( init_val == 0) { return 0; }
        } else {
            init_val = ConstantFP::get(getGlobalContext(), APFloat(0.0));
        }

        AllocaInst *alloca = CreateEntryBlockAlloca(func, var_name);
        Builder.CreateStore(init_val, alloca);

        old_bindings.push_back(named_values[var_name]);

        named_values[var_name] = alloca;
    }

    Value *body_val = body->codegen();
    if ( body_val == 0 ) { return 0; }

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        named_values[var_names[i].first] = old_bindings[i];
    }

    return body_val;
}

Value *IfNode::codegen() {
    Value *cond_value = condition->codegen();
    if ( cond_value == 0 ) { return 0; }

    cond_value = Builder.CreateFCmpONE(cond_value, ConstantFP::get(getGlobalContext(), APFloat(0.0)), "ifcond");

    Function *func = Builder.GetInsertBlock()->getParent();

    BasicBlock *then_block = BasicBlock::Create(getGlobalContext(), "then", func);
    BasicBlock *else_block = BasicBlock::Create(getGlobalContext(), "else");
    BasicBlock *merge_block = BasicBlock::Create(getGlobalContext(), "ifcont");

    Builder.CreateCondBr(cond_value, then_block, else_block);
    Builder.SetInsertPoint(then_block);

    Value *then_value = then->codegen();
    if ( then_value == 0 ) { return 0; }


    Builder.CreateBr(merge_block);
    then_block = Builder.GetInsertBlock();

    func->getBasicBlockList().push_back(else_block);
    Builder.SetInsertPoint(else_block);

    Value *else_value = _else->codegen();
    if ( else_value == 0 ) { return 0; }

    Builder.CreateBr(merge_block);
    else_block = Builder.GetInsertBlock();

    func->getBasicBlockList().push_back(merge_block);
    Builder.SetInsertPoint(merge_block);
    PHINode *node = Builder.CreatePHI(Type::getDoubleTy(getGlobalContext()), 2, "iftmp");

    node->addIncoming(then_value, then_block);
    node->addIncoming(else_value, else_block);

    return node;
}

Value *ForNode::codegen() {
    Function *func = Builder.GetInsertBlock()->getParent();

    AllocaInst *alloca = CreateEntryBlockAlloca(func, var_name);

    Value *start_value = start->codegen();
    if ( start_value == 0 ) { return 0; }

    Builder.CreateStore(start_value, alloca);

    BasicBlock *loop_block = BasicBlock::Create(getGlobalContext(), "loop", func);

    Builder.CreateBr(loop_block);
    Builder.SetInsertPoint(loop_block);

    AllocaInst *old_value = named_values[var_name];
    named_values[var_name] = alloca;

    if ( body->codegen() == 0 ) { return 0; }

    Value *step_value;
    if ( step ) {
        step_value = step->codegen();
        if ( step_value == 0 ) { return 0; }
    } else {
        step_value = ConstantFP::get(getGlobalContext(), APFloat(1.0));
    }

    Value *end_condition = end->codegen();
    if ( end_condition == 0 ) { return 0; }

    Value *current_var = Builder.CreateLoad(alloca, var_name.c_str());
    Value *next_var = Builder.CreateFAdd(current_var, step_value, "nextvar");
    Builder.CreateStore(next_var, alloca);

    end_condition = Builder.CreateFCmpONE(
        end_condition,
        ConstantFP::get(getGlobalContext(), APFloat(0.0)),
        "loopcond"
    );

    BasicBlock *after_block = BasicBlock::Create(getGlobalContext(), "afterloop", func);

    Builder.CreateCondBr(end_condition, loop_block, after_block);
    Builder.SetInsertPoint(after_block);

    if ( old_value ) {
        named_values[var_name] = old_value;
    } else {
        named_values.erase(var_name);
    }

    return Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
}


static void HandleDefinition() {
  if ( FunctionNode *node = ParseDefinition() ) {
      if ( Function *func = node->codegen() ) {
          fprintf(stderr, "Read function definition:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void HandleExtern() {
  if ( PrototypeNode *node = ParseExtern() ) {
      if ( Function *func = node->codegen() ) {
          fprintf(stderr, "Read extern:");
          func->dump();
      }
  } else {
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if ( FunctionNode *node = ParseTopLevelExpression() ) {
      if ( Function *func = node->codegen() ) {
          void *func_ptr = engine->getPointerToFunction(func);

          double (*func_pointer)() = (double (*)())(intptr_t)func_ptr;

          fprintf(stderr, "Evaluated to: %f\n", func_pointer());
      }
  } else {
    getNextToken();
  }
}

static void MainLoop() {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (current_token) {
        case tok_eof: return;
        case ';':
            getNextToken();
            break;
        case tok_def:
            HandleDefinition();
            break;
        case tok_extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

int main() {
    InitializeNativeTarget();

    LLVMContext &context = getGlobalContext();

    op_precedence['='] = 2;
    op_precedence['<'] = 10;
    op_precedence['+'] = 20;
    op_precedence['-'] = 20;
    op_precedence['*'] = 40;

    fprintf(stderr, "ready> ");
    getNextToken();

    module = new Module("my cool jit", context);

    std::string error_string;
    engine = EngineBuilder(module).setErrorStr(&error_string).create();
    if ( ! engine ) {
        fprintf(
                stderr,
                "Could not create execution engine: %s\n",
                error_string.c_str()
                );
        exit(1);
    }

    FunctionPassManager pass_manager(module);

    pass_manager.add(new DataLayout(*engine->getDataLayout()));
    pass_manager.add(createBasicAliasAnalysisPass());
    pass_manager.add(createPromoteMemoryToRegisterPass());
    pass_manager.add(createInstructionCombiningPass());
    pass_manager.add(createReassociatePass());
    pass_manager.add(createGVNPass());
    pass_manager.add(createCFGSimplificationPass());

    pass_manager.doInitialization();

    func_pass_manager = &pass_manager;

    MainLoop();

    func_pass_manager = 0;

    module->dump();

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
