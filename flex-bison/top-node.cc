#include "top-node.hh"
using namespace top;

// --------------- NODE --------------- //

node::~node() {}

// --------------- BODY_NODE --------------- //

body_node::body_node() : definitions() {}

body_node::~body_node() {
    for (auto node : definitions)
        delete node;
    definitions.clear();
}

void body_node::add(funcdef_node* f) {
    if (f != NULL)
        definitions.push_back(f);
}

void body_node::add(vardef_node* v) {
    if (v != NULL)
        definitions.push_back(v);
}

void body_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Body ("
        << definitions.size() << ")" << endl;
    if (!flat)
        for (auto node : definitions)
            node->print(out, depth + 1, flat);
}

llvm::Value* body_node::generate() const {
    
}

// --------------- FUNCDEF_NODE --------------- //

funcdef_node::funcdef_node(const string& n, const string& ret_t, funcparams_node* p, block_node* b)
: name(n), return_type(ret_t), params(p), block(b) {}

funcdef_node::~funcdef_node() {
    if (params != NULL) delete params;
    if (block != NULL) delete block;
}

void funcdef_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Function definition: "
		<< name << ", returns " << return_type << endl;
    if (!flat) {
        if (params != NULL) params->print(out, depth + 1, flat);
        if (block != NULL) block->print(out, depth + 1, flat);
    }
}

llvm::Value* funcdef_node::generate() const {
    vector<string> names;
    vector<llvm::Type*> types;
    for (auto p = params->cbegin(); p != params->cend(); ++p) {
        names.push_back((*p)->getName());
        types.push_back(typeMap[(*p)->getType()]);
    }
    
    llvm::FunctionType* func_type = llvm::FunctionType::get(typeMap[return_type], types, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name, &module);
    
    unsigned int idx = 0U;
    for (auto& arg : func->args())
        arg.setName(names[idx]);
    
    llvm::BasicBlock* bBlock = llvm::BasicBlock::Create(llvmContext, "entry", func);
    irBuilder.SetInsertPoint(bBlock);
    
    for (auto p = block->cbegin(); p != block->cend(); ++p)
        (*p)->generate();
    
    llvm::verifyFunction(*func);
    return func;
}

// --------------- FUNCPARAMS_NODE --------------- //

funcparams_node::funcparams_node() : params() {}

funcparams_node::~funcparams_node() {
    for (auto node : params)
        delete node;
    params.clear();
}

vector<funcparam_node*>::const_iterator funcparams_node::cbegin() const {
    return params.cbegin();
}

vector<funcparam_node*>::const_iterator funcparams_node::cend() const {
    return params.cend();
}

void funcparams_node::add(funcparam_node* param) {
    if (param != NULL)
        params.insert(params.begin(), param);
}

void funcparams_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Function parameters ("
        << params.size() << ")" << endl;
    if (!flat)
        for (auto node : params)
            node->print(out, depth + 1, flat);
}

llvm::Value* funcparams_node::generate() const {
    return nullptr;
}

// --------------- FUNCPARAM_NODE --------------- //

funcparam_node::funcparam_node(const string& n, const string& t) : name(n), type(t) {}

const string& funcparam_node::getName() const {
    return name;
}

const string& funcparam_node::getType() const {
    return type;
}

funcparam_node::~funcparam_node() {}

void funcparam_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Function parameter: "
        << name << " (" << type << ")" << endl;
}

llvm::Value* funcparam_node::generate() const {
    return nullptr;
}

// --------------- BLOCK_NODE --------------- //

block_node::block_node() : statements() {}

block_node::~block_node() {
    for (auto node : statements)
        delete node;
    statements.clear();
}

vector<statement_node*>::const_iterator block_node::cbegin() const {
    return statements.cbegin();
}

vector<statement_node*>::const_iterator block_node::cend() const {
    return statements.cend();
}

void block_node::add(statement_node* s) {
    if (s != NULL)
        statements.push_back(s);
}

void block_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Block ("
        << statements.size() << ")" << endl;
    if (!flat)
        for (auto node : statements)
            node->print(out, depth + 1, flat);
}

llvm::Value* block_node::generate() const {
    return nullptr;
}

// --------------- STATEMENT_NODE --------------- //

statement_node::~statement_node() {}

// --------------- VARDEF_NODE --------------- //

vardef_node::vardef_node(const string& n, const string& t, expression_node* e) : name(n), type(t), expr(e) {}

vardef_node::~vardef_node() {
    if (expr != NULL) delete expr;
}

void vardef_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Variable definition: "
        << name << " (" << type << ")" << endl;
    if (!flat && expr != NULL) expr->print(out, depth + 1, flat);
}

llvm::Value* vardef_node::generate() const {
    
}

// --------------- RETURN_NODE --------------- //

return_node::return_node(expression_node* e) : expr(e) {}

return_node::~return_node() {
    if (expr != NULL) delete expr;
}

void return_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Return" << endl;
    if (!flat && expr != NULL) expr->print(out, depth + 1, flat);
}

llvm::Value* return_node::generate() const {
    return irBuilder.CreateRet(expr->generate());
}

// --------------- ASSIGNMENT_NODE --------------- //

assignment_node::assignment_node(const string& n, expression_node* e) : name(n), expr(e) {}

assignment_node::~assignment_node() {
    if (expr != NULL) delete expr;
}

void assignment_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Assignment to "
        << name << endl;
    if (!flat && expr != NULL) expr->print(out, depth + 1, flat);
}

llvm::Value* assignment_node::generate() const {
    // TODO: Assigment
    return nullptr;
}

// --------------- EXPR_WRAPPER_NODE --------------- //

expr_wrapper_node::expr_wrapper_node(expression_node* e) : expr(e) {}

expr_wrapper_node::~expr_wrapper_node() {
    if (expr != NULL) delete expr;
}

void expr_wrapper_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Statement" << endl;
    if (!flat && expr != NULL) expr->print(out, depth + 1, flat);
}

llvm::Value* expr_wrapper_node::generate() const {
    return expr->generate();
}

// --------------- EXPRESSION_NODE --------------- //

expression_node::~expression_node() {}

// --------------- FUNCCALL_NODE --------------- //

funccall_node::funccall_node(const string& n, params_node* p) : name(n), params(p) {}

funccall_node::~funccall_node() {
    if (params != NULL) delete params;
}

void funccall_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Function call to "
        << name << endl;
    if (!flat && params != NULL) params->print(out, depth + 1, flat);
}

llvm::Value* funccall_node::generate() const {
    llvm::Function* function = module.getFunction(name);
    
    if (!function) return nullptr;
    if (function->arg_size() != params->size()) return nullptr;
    
    vector<llvm::Value*> args;
    for (vector<expression_node*>::const_iterator iter = params->cbegin(); iter != params->cend(); ++iter) {
        args.push_back((*iter)->generate());
    }
    
    return irBuilder.CreateCall(function, args, "calltmp");
}

// --------------- PARAMS_NODE --------------- //

params_node::params_node() : params() {}

params_node::~params_node() {}

void params_node::add(expression_node* e) {
    if (e != NULL)
        params.insert(params.begin(), e);
}

size_t params_node::size() const {
    return params.size();
}

vector<expression_node*>::const_iterator params_node::cbegin() const {
    return params.cbegin();
}

vector<expression_node*>::const_iterator params_node::cend() const {
    return params.cend();
}

void params_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Parameters" << endl;
    if (!flat)
        for (auto& node : params)
            node->print(out, depth + 1, flat);
}

llvm::Value* params_node::generate() const {
    // TODO: What is the use of that anyway?
    return nullptr;
}

// --------------- BINOP_NODE --------------- //

binop_node::binop_node(expression_node* l, const string& o, expression_node* r) : op(o), left(l), right(r) {}

binop_node::~binop_node() {
    if (left != NULL) delete left;
    if (right != NULL) delete right;
}

void binop_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Binary operation (operator '"
        << op << "')" << endl;
    if (!flat) {
        if (left != NULL) left->print(out, depth + 1, flat);
        if (right != NULL) right->print(out, depth + 1, flat);
    }
}

llvm::Value* binop_node::generate() const {
    llvm::Value* lval = left->generate();
    llvm::Value* rval = right->generate();
    
    // TODO: Switch on type!
    if (op == "+") return irBuilder.CreateFAdd(lval, rval, "addtmp");
    else if (op == "-") return irBuilder.CreateFSub(lval, rval, "subtmp");
    else if (op == "*") return irBuilder.CreateFMul(lval, rval, "multmp");
    else if (op == "/") return irBuilder.CreateFDiv(lval, rval, "divtmp");
    else return nullptr;
}

// --------------- UNOP_NODE --------------- //

unop_node::unop_node(const string& o, expression_node* e) : op(o), expr(e) {}

unop_node::~unop_node() {
	if (expr != NULL) delete expr;
}

void unop_node::print(ostream& out, unsigned int depth, bool flat) const {
	out << string(depth * 2, ' ') << "-> Unary operation (operator '"
		<< op << "')" << endl;
	if (!flat && expr != NULL)
		expr->print(out, depth + 1, flat);
}

llvm::Value* unop_node::generate() const {
    // TODO: Switch on operations, types
    return nullptr;
}

// --------------- VARIABLE_NODE --------------- //

variable_node::variable_node(const string& n) : name(n) {}

variable_node::~variable_node() {}

void variable_node::print(ostream& out, unsigned int depth, bool flat) const {
    out << string(depth * 2, ' ') << "-> Variable " << name << endl;
}

llvm::Value* variable_node::generate() const {
    // TODO: Generate variable
    return nullptr;
}

// --------------- INT_NODE --------------- //

int_node::int_node(int v) : value(v) { }

int_node::~int_node() { }

void int_node::print(ostream& out, unsigned int depth, bool flat) const {
	out << string(depth * 2, ' ') << "-> Literal: " << value << endl;
}

llvm::Value* int_node::generate() const {
    return llvm::ConstantInt::get(llvmContext, llvm::APInt(32U, value, true));
}

// --------------- FLOAT_NODE --------------- //

float_node::float_node(double v) : value(v) { }

float_node::~float_node() { }

void float_node::print(ostream& out, unsigned int depth, bool flat) const {
	out << string(depth * 2, ' ') << "-> Literal: " << value << endl;
}

llvm::Value* float_node::generate() const {
	return llvm::ConstantFP::get(llvmContext, llvm::APFloat(value));
}

// --------------- STRING_NODE --------------- //

string_node::string_node(const string& v) : value(v) { }

string_node::~string_node() { }

void string_node::print(ostream& out, unsigned int depth, bool flat) const {
	out << string(depth * 2, ' ') << "-> Literal: " << value << endl;
}

llvm::Value* string_node::generate() const {
	return nullptr;
}

