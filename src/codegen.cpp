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

#include <string>
#include <vector>

#include "ast.h"
#include "lexer.h"
#include "errors.h"
#include "context.h"

using namespace llvm;


static AllocaInst *
CreateEntryBlockAlloca(Context *context,
                       Function *func,
                       const std::string &var_name) {
    IRBuilder<> tmp_builder(&func->getEntryBlock(),
                            func->getEntryBlock().begin());

    return tmp_builder.CreateAlloca(Type::getDoubleTy(context->llvm_context()), 0, var_name.c_str());
}

Value *
NumberNode::codegen(Context *context) {
    return ConstantFP::get(context->llvm_context(), APFloat(val));
}

Value *
VariableNode::codegen(Context *context) {
    Value *val = context->get_named_value(name);
    if ( !val ) {
        return ErrorV("Unknown variable name");
    }

    return context->builder()->CreateLoad(val, name.c_str());
}

Value *
BinaryNode::codegen(Context *context) {
    if ( op == '=' ) {
        VariableNode *lhse = dynamic_cast<VariableNode*>(lhs);
        if ( ! lhse ) {
            return ErrorV("destination of '=' must be a variable");
        }

        Value *val = rhs->codegen(context);
        if ( val == 0 ) { return 0; }

        Value *variable = context->get_named_value(lhse->getName());
        if ( variable == 0 ) {
            return ErrorV("Unknown variable name");
        }

        context->builder()->CreateStore(val, variable);

        return val;
    }

    Value *left = lhs->codegen(context);
    Value *right = rhs->codegen(context);

    if (left == 0 || right == 0 ) { return 0; }

    switch (op) {
    case '+': return context->builder()->CreateFAdd(left, right, "addtmp");
    case '-': return context->builder()->CreateFSub(left, right, "subtmp");
    case '*': return context->builder()->CreateFMul(left, right, "multmp");
    case '<':
        left = context->builder()->CreateFCmpULT(left, right, "cmptmp");
        return context->builder()->CreateUIToFP(left,
                                                Type::getDoubleTy(context->llvm_context()), "booltmp");
    default: break;
    }

    Function *func = context->module()->getFunction(std::string("binary") + op);
    assert(func && "binary operator not found!");

    Value *operands[2] = { left, right };

    return context->builder()->CreateCall(func, operands, "binop");
}

Value *
UnaryNode::codegen(Context *context) {
    Value *operand_value = operand->codegen(context);
    if ( operand_value == 0 ) { return 0; }

    Function *func = context->module()->getFunction(std::string("unary") + opcode);
    if ( func == 0 ) {
        return ErrorV("Unknown unary operator");
    }

    return context->builder()->CreateCall(func, operand_value, "unop");
}

Value *
CallNode::codegen(Context *context) {
    Function *callee_func = context->module()->getFunction(callee);
    if ( callee_func == 0 ) {
        return ErrorV("Unknown function referenced");
    }

    if ( callee_func->arg_size() != args.size() ) {
        return ErrorV("Incorrect number of arguments passed");
    }

    std::vector<Value*> arg_values;
    for ( unsigned i = 0, num_args = args.size(); i < num_args; ++i ) {
        arg_values.push_back(args[i]->codegen(context));
        if ( arg_values.back() == 0 ) { return 0; }
    }

    return context->builder()->CreateCall(callee_func, arg_values, "calltmp");
}

Function *
PrototypeNode::codegen(Context *context) {
    std::vector<Type*> doubles(args.size(),
                               Type::getDoubleTy(context->llvm_context()));
    FunctionType *func_type = FunctionType::get(Type::getDoubleTy(context->llvm_context()),
                                                doubles, false);

    Function *func = Function::Create(func_type,
                                      Function::ExternalLinkage,
                                      name,
                                      context->module());

    if ( func->getName() != name ) {
        func->eraseFromParent();
        func = context->module()->getFunction(name);

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
PrototypeNode::CreateArgumentAllocas(Context *context, Function *func) {
    Function::arg_iterator iterator = func->arg_begin();

    for (unsigned i = 0, size = args.size(); i != size; ++i, ++iterator) {
        AllocaInst *alloca = CreateEntryBlockAlloca(context, func, args[i]);
        context->builder()->CreateStore(iterator, alloca);
        context->set_named_value(args[i], alloca);
    }

}

Function *
FunctionNode::codegen(Context *context) {
    context->clear_all_named_values();

    Function *func = proto->codegen(context);
    if ( func == 0 ) { return 0; }

    if ( proto->isBinaryOp() ) {
        op_precedence[proto->getOperatorName()] = proto->getBinaryPrecedence();
    }

    BasicBlock *block = BasicBlock::Create(context->llvm_context(),
                                           "entry",
                                           func);
    context->builder()->SetInsertPoint(block);

    proto->CreateArgumentAllocas(context, func);

    if ( Value *retval = body->codegen(context) ) {
        context->builder()->CreateRet(retval);
        verifyFunction(*func);

        context->pass_manager()->run(*func);

        return func;
    }

    func->eraseFromParent();

    if ( proto->isBinaryOp() ) {
        op_precedence.erase(proto->getOperatorName());
    }

    return 0;
}

Value *
VarNode::codegen(Context *context) {
    std::vector<AllocaInst *> old_bindings;

    Function *func = context->builder()->GetInsertBlock()->getParent();

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        const std::string &var_name = var_names[i].first;
        ASTNode *init = var_names[i].second;

        Value *init_val;
        if ( init ) {
            init_val = init->codegen(context);
            if ( init_val == 0) { return 0; }
        } else {
            init_val = ConstantFP::get(context->llvm_context(), APFloat(0.0));
        }

        AllocaInst *alloca = CreateEntryBlockAlloca(context, func, var_name);
        context->builder()->CreateStore(init_val, alloca);

        old_bindings.push_back(context->get_named_value(var_name));
        context->set_named_value(var_name, alloca);
    }

    Value *body_val = body->codegen(context);
    if ( body_val == 0 ) { return 0; }

    for (unsigned i = 0, size = var_names.size(); i != size; ++i) {
        context->set_named_value(var_names[i].first, old_bindings[i]);
    }

    return body_val;
}

Value *
IfNode::codegen(Context *context) {
    Value *cond_value = condition->codegen(context);
    if ( cond_value == 0 ) { return 0; }

    cond_value = context->builder()->CreateFCmpONE(cond_value, ConstantFP::get(context->llvm_context(), APFloat(0.0)), "ifcond");

    Function *func = context->builder()->GetInsertBlock()->getParent();

    BasicBlock *then_block = BasicBlock::Create(context->llvm_context(), "then", func);
    BasicBlock *else_block = BasicBlock::Create(context->llvm_context(), "else");
    BasicBlock *merge_block = BasicBlock::Create(context->llvm_context(), "ifcont");

    context->builder()->CreateCondBr(cond_value, then_block, else_block);
    context->builder()->SetInsertPoint(then_block);

    Value *then_value = then->codegen(context);
    if ( then_value == 0 ) { return 0; }


    context->builder()->CreateBr(merge_block);
    then_block = context->builder()->GetInsertBlock();

    func->getBasicBlockList().push_back(else_block);
    context->builder()->SetInsertPoint(else_block);

    Value *else_value = _else->codegen(context);
    if ( else_value == 0 ) { return 0; }

    context->builder()->CreateBr(merge_block);
    else_block = context->builder()->GetInsertBlock();

    func->getBasicBlockList().push_back(merge_block);
    context->builder()->SetInsertPoint(merge_block);
    PHINode *node = context->builder()->CreatePHI(Type::getDoubleTy(context->llvm_context()), 2, "iftmp");

    node->addIncoming(then_value, then_block);
    node->addIncoming(else_value, else_block);

    return node;
}

Value *
ForNode::codegen(Context *context) {
    Function *func = context->builder()->GetInsertBlock()->getParent();

    AllocaInst *alloca = CreateEntryBlockAlloca(context, func, var_name);

    Value *start_value = start->codegen(context);
    if ( start_value == 0 ) { return 0; }

    context->builder()->CreateStore(start_value, alloca);

    BasicBlock *loop_block = BasicBlock::Create(context->llvm_context(), "loop", func);

    context->builder()->CreateBr(loop_block);
    context->builder()->SetInsertPoint(loop_block);

    AllocaInst *old_value = context->get_named_value(var_name);
    context->set_named_value(var_name, alloca);

    if ( body->codegen(context) == 0 ) { return 0; }

    Value *step_value;
    if ( step ) {
        step_value = step->codegen(context);
        if ( step_value == 0 ) { return 0; }
    } else {
        step_value = ConstantFP::get(context->llvm_context(), APFloat(1.0));
    }

    Value *end_condition = end->codegen(context);
    if ( end_condition == 0 ) { return 0; }

    Value *current_var = context->builder()->CreateLoad(alloca, var_name.c_str());
    Value *next_var = context->builder()->CreateFAdd(current_var, step_value, "nextvar");
    context->builder()->CreateStore(next_var, alloca);

    end_condition = context->builder()->CreateFCmpONE(
        end_condition,
        ConstantFP::get(context->llvm_context(), APFloat(0.0)),
        "loopcond"
    );

    BasicBlock *after_block = BasicBlock::Create(context->llvm_context(), "afterloop", func);

    context->builder()->CreateCondBr(end_condition, loop_block, after_block);
    context->builder()->SetInsertPoint(after_block);

    if ( old_value ) {
        context->set_named_value(var_name, old_value);
    } else {
        context->clear_named_value(var_name);
    }

    return Constant::getNullValue(Type::getDoubleTy(context->llvm_context()));
}
