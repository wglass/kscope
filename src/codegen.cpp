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

#include <map>
#include <string>
#include <vector>

#include "ast.h"
#include "lexer.h"
#include "errors.h"
#include "codegen.h"

using namespace llvm;

Module *module;
ExecutionEngine *engine;
FunctionPassManager *func_pass_manager;

std::map<std::string, AllocaInst*> named_values;
IRBuilder<> Builder(getGlobalContext());


static AllocaInst *
CreateEntryBlockAlloca(Function *func, const std::string &var_name) {
    IRBuilder<> TmpB(&func->getEntryBlock(), func->getEntryBlock().begin());

    return TmpB.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0, var_name.c_str());
}

Value *
NumberNode::codegen() {
    return ConstantFP::get(getGlobalContext(), APFloat(val));
}

Value *
VariableNode::codegen() {
    Value *val = named_values[name];
    if ( !val ) {
        return ErrorV("Unknown variable name");
    }

    return Builder.CreateLoad(val, name.c_str());
}

Value *
BinaryNode::codegen() {
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

Value *
UnaryNode::codegen() {
    Value *operand_value = operand->codegen();
    if ( operand_value == 0 ) { return 0; }

    Function *func = module->getFunction(std::string("unary") + opcode);
    if ( func == 0 ) {
        return ErrorV("Unknown unary operator");
    }

    return Builder.CreateCall(func, operand_value, "unop");
}

Value *
CallNode::codegen() {
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

Function *
PrototypeNode::codegen() {
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

void
PrototypeNode::CreateArgumentAllocas(Function *func) {
    Function::arg_iterator iterator = func->arg_begin();

    for (unsigned i = 0, size = args.size(); i != size; ++i, ++iterator) {
        AllocaInst *alloca = CreateEntryBlockAlloca(func, args[i]);
        Builder.CreateStore(iterator, alloca);
        named_values[args[i]] = alloca;
    }

}

Function *
FunctionNode::codegen() {
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

Value *
VarNode::codegen() {
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

Value *
IfNode::codegen() {
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

Value *
ForNode::codegen() {
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
