#include "top-context.hh"
using namespace top;

top::context::context(top::context* rt) : functions(), variables(), root(rt), leaves() { }

top::context::~context() {
	for (auto leave : leaves)
		if (leave != nullptr)
			delete leave;
	leaves.clear();
	
	root = nullptr;
}

bool top::context::is_root() const {
	return root == nullptr;
}

top::context* top::context::add_context() {
	top::context* cxt = new top::context(this);
	leaves.push_back(cxt);	
	return cxt;
}

llvm::Function* top::context::get_function(const string& name) const {
	auto func = functions.find(name);
	if (func == functions.end()) {
		if (root != nullptr)
			return root->get_function(name);
		else
			return nullptr;
	}
	return func->second;
}

llvm::AllocaInst* top::context::get_variable(const string& name) const {
	auto var = variables.find(name);
	if (var == variables.end()) {
		if (root != nullptr)
			return root->get_variable(name);
		else
			return nullptr;
	}
	return var->second;
}

bool top::context::add_function(const string& name, llvm::Function* function) {
	if (functions.find(name) != functions.end())
		return false;
	functions[name] = function;
	return true;
}

bool top::context::add_variable(const string& name, llvm::AllocaInst* variable) {
	if (variables.find(name) != variables.end())
		return false;
	variables[name] = variable;
	return true;
}
