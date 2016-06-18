#ifndef GO_CONTEXT_HH
#define GO_CONTEXT_HH

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <string>
#include <map>
#include <vector>

namespace top {
	using std::string;
	using std::map;
	using std::vector;

	class context {
		map<string, llvm::Function*> functions;
		map<string, llvm::AllocaInst*> variables;
		
		context* root;
		vector<context*> leaves;

	public:
		context(context* rt);
		~context();

		bool is_root() const;
		context* add_context();

		llvm::Function* get_function(const string& name) const;
		llvm::AllocaInst* get_variable(const string& name) const;

		bool add_function(const string& name, llvm::Function* function);
		bool add_variable(const string& name, llvm::AllocaInst* variable);
	};
}

#endif
