#ifndef GO_NODE_HH
#define GO_NODE_HH

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

#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace top {
    using std::ostream;
    using std::endl;
    using std::string;
    using std::vector;
    using std::map;
    
    class node;
    class body_node;
    class funcdef_node;
    class funcparams_node;
    class funcparam_node;
    class block_node;

    class statement_node;
    class vardef_node;
    class return_node;
    class assignment_node;
    class expr_wrapper_node;

    class expression_node;
    class funccall_node;
    class params_node;
    class binop_node;

    static llvm::LLVMContext llvmContext;
    static llvm::IRBuilder<> irBuilder(llvmContext);
    static llvm::Module module("Gandalf", llvmContext);
    static map<std::string, llvm::Type*> typeMap;
    
    class node {
    public:
        virtual ~node();
        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const = 0;
        virtual llvm::Value* generate() const = 0;
    };
        
    class expression_node : public node {
    public:
        virtual ~expression_node();
    };
	
    class int_node : public expression_node {
            int value;
    public:
        int_node(int v);
        virtual ~int_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };


    class float_node : public expression_node {
        double value;
    public:
        float_node(double v);
        virtual ~float_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class string_node : public expression_node {
        string value;
    public:
        string_node(const string& v);
        virtual ~string_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };
    
    class body_node : public node {
        vector<node*> definitions;
    public:
        body_node();
        virtual ~body_node();

        void add(funcdef_node* v);
        void add(vardef_node* f);

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class funcdef_node : public node {
        string name;
		string return_type;
        funcparams_node* params;
        block_node* block;
    public:
        funcdef_node(const string& n, const string& ret_t, funcparams_node* p, block_node* b);
        virtual ~funcdef_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class funcparams_node : public node {
        vector<funcparam_node*> params;
    public:
        funcparams_node();
        virtual ~funcparams_node();

        vector<funcparam_node*>::const_iterator cbegin() const;
        vector<funcparam_node*>::const_iterator cend() const;
        
        void add(funcparam_node* param);
        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class funcparam_node : public node {
        string name;
        string type;
    public:
        funcparam_node(const string& n, const string& t);
        virtual ~funcparam_node();

        const string& getName() const;
        const string& getType() const;
        
        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class block_node : public node {
        vector<statement_node*> statements;
    public:
        block_node();
        virtual ~block_node();

        vector<statement_node*>::const_iterator cbegin() const;
        vector<statement_node*>::const_iterator cend() const;
        void add(statement_node* s);

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class statement_node : public node {
    public:
        virtual ~statement_node();
    };

    class vardef_node : public statement_node {
        string name;
        string type;
        expression_node* expr;
    public:
        vardef_node(const string& n, const string& t, expression_node* e);
        virtual ~vardef_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class return_node : public statement_node {
        expression_node* expr;
    public:
        return_node(expression_node* e);
        virtual ~return_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class assignment_node : public statement_node {
        string name;
        expression_node* expr;
    public:
        assignment_node(const string& n, expression_node* e);
        virtual ~assignment_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class expr_wrapper_node : public statement_node {
        expression_node* expr;
    public:
        expr_wrapper_node(expression_node* e);
        virtual ~expr_wrapper_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };
      
    class funccall_node : public expression_node {
        string name;
        params_node* params;
    public:
        funccall_node(const string& n, params_node* p);
        virtual ~funccall_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class params_node : public node {
        vector<expression_node*> params;
    public:
        params_node();
        virtual ~params_node();

        void add(expression_node* e);
        size_t size() const;
        
        vector<expression_node*>::const_iterator cbegin() const;
        vector<expression_node*>::const_iterator cend() const;

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class binop_node : public expression_node {
        string op;
        expression_node* left;
        expression_node* right;
    public:
        binop_node(expression_node* l, const string& o, expression_node* r);
        virtual ~binop_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };
	
    class unop_node : public expression_node {
        string op;
        expression_node* expr;
    public:
        unop_node(const string& o, expression_node* e);
        virtual ~unop_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };

    class variable_node : public expression_node {
        string name;
    public:
        variable_node(const string& n);
        virtual ~variable_node();

        virtual void print(ostream& out, unsigned int depth = 0U, bool flat = false) const;
        virtual llvm::Value* generate() const;
    };
}

#endif
