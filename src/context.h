#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/PassManager.h"

#include <map>
#include <string>


class Context {
    llvm::Module *module_;
    llvm::ExecutionEngine *engine_;
    llvm::FunctionPassManager *pass_manager_;
    llvm::IRBuilder<> *builder_;

    std::map<std::string, llvm::AllocaInst*> named_values;

    Context(llvm::Module *module);
    Context(const Context &other);
    Context &operator =(const Context &other);

    void initialize(llvm::Module *module);

public:
    Context();
    ~Context();

    const llvm::Module *module() const;
    const llvm::ExecutionEngine *engine() const;
    const llvm::FunctionPassManager *pass_manager() const;
    const llvm::IRBuilder<> *builder() const;

    llvm::AllocaInst * get_named_value(const std::string &name);
    void set_named_value(const std::string &name, llvm::AllocaInst* value);
    void clear_named_value(const std::string &name);
    void clear_all_named_values();
};
