#ifndef GO_DRIVER_HH
#define GO_DRIVER_HH

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
#include <memory>
#include "top-parser.hh"
#include "top-node.hh"
#include "top-symbol.hh"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::top_parser::symbol_type yylex (top_driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing of top.
class top_driver {
public:
	top_driver ();
	virtual ~top_driver ();

	std::map<std::string, int> variables;
	std::vector<std::string> imports;
	std::string package;

	std::vector<top::symbol> symbols;
	top::body_node* body;

        std::unique_ptr<llvm::Module> module;

	std::string result;

	// Handling the scanner.
	void scan_begin();
	void scan_end();
	bool trace_scanning;
	// Run the parser on file F.
	// Return 0 on success.
	int parse (const std::string& f);
	bool createTypes();
	// The name of the file being parsed.
	// Used later to pass the file name to the location tracker.
	std::string file;
	// Whether parser traces should be generated.
	bool trace_parsing;

	bool hasError;

	// Error handling.
	void error (const yy::location& l, const std::string& m);
	void error (const std::string& m);
};
#endif // ! top_DRIVER_HH
