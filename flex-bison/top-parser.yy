
%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.2"
%defines
%define parser_class_name {top_parser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "top-node.hh"
#include "top-symbol.hh"
class top_driver;
}

// The parsing context.
%param { top_driver& driver }

%locations
%initial-action
{
	// Initialize the initial location.
	@$.begin.filename = @$.end.filename = &driver.file;
};

%define parse.trace
%define parse.error verbose

%code
{
# include "top-driver.hh"
}

%define api.token.prefix {TOK_}
%token
	END	0   "eof"
	PACKAGE "package"
	IMPORT	"import"
	FUNCTION "function"
	PARAMETERS "parameters"
	VAR	    "var"
	RETURN 	"return"
	ENUM 	"enum"
	OBJ		"obj"
	IF		"if"
	NEW		"new"
	WHILE	"while"
	ENDSTUFF "end"
	FOR		"for"
	DO		"do"
	ARROW	"->"
	ASSIGN	"="
	MINUS	"-"
	PLUS	"+"
	STAR	"*"
	SLASH	"/"
	LPAREN	"("
	RPAREN	")"
	LBLOCK	"{"
	RBLOCK	"}"
	COMMA	","
;

%token 	<std::string> TIDENTIFIER "tidentifier"
%token	<std::string> IDENTIFIER "identifier"
%token	<std::string> STRING "stringliteral"
%token	<int> INT "int"
%token	<double> NUMBER "number"

%type	<std::string> arrow
%type	<std::string> funcbody
%type	<top::expression_node*> exp
%type 	<top::statement_node*> statement
%type	<top::funcdef_node*> funcdef
%type 	<top::vardef_node*> vardef
%type 	<top::funcparams_node*> funcparams
%type 	<top::funcparam_node*> funcparam
%type 	<top::block_node*> block
%type 	<top::params_node*> params

%printer { yyoutput << $$; } <*>;

%%

%start unit;

unit:
	global {}
|	%empty { std::cout << "Empty File!" << std::endl; }

global:
	global function { std::cout << "FUNCTION" << std::endl; }
|	global enum { std::cout << "ENUM" << std::endl; }
|	global obj	 { std::cout << "OBJ" << std::endl; }
|	global var	 { std::cout << "VAR" << std::endl; }
|	%empty { }

enum:
	"enum" "identifier" enumbody ENDSTUFF { std::cout << $2 << std::endl; }
|	"enum" "tidentifier" enumbody ENDSTUFF { std::cout << $2 << std::endl; }

enumbody:
	enumbody "identifier"	{ std::cout << "MEMBER->" << $2 << std::endl; }
|	enumbody "tidentifier"	{ std::cout << "MEMBER->" << $2 << std::endl; }
|	%empty					{ }

obj:
	"obj" "identifier" objbody ENDSTUFF { std::cout << $2 << std::endl; }
|	"obj" "tidentifier" objbody ENDSTUFF { std::cout << $2 << std::endl; }

objbody:
	objbody "tidentifier"	{ std::cout << "MEMBER->" << $2 << std::endl; }
|	%empty					{ }

var:
	"var" vardef { }

vardef:
  	"tidentifier" "=" exp	{ std::cout << "variable with expression " << $1 << std::endl; }
|	"tidentifier" { std::cout << "default init variable " << $1 << std::endl; }

function:
	"function" "identifier" "parameters" LPAREN funcparamstart RPAREN funcbody ENDSTUFF { std::cout << $2 << std::endl; }
|	"function" "tidentifier" "parameters" LPAREN funcparamstart RPAREN funcbody ENDSTUFF {
	std::cout << $2 << std::endl; }

funcparamstart:
	funcparam		{}
|	%empty	{}

funcparam:
	funcparam "," "tidentifier" { std::cout << "PARAM->" << $3 << std::endl; }
|	"tidentifier" { std::cout << "PARAM->" << $1 << std::endl; }

funcbody:
	funcbody statement			{ }
|	%empty	{}

statement:
	var						{ }
|	"return" exp			{ std::cout << "return statement!" << std::endl; }
|	"tidentifier" "=" exp	{ std::cout << "assign" << std::endl; }
| 	exp						{ }

%left "+" "-";
%left "*" "/";
%right UMINUS;

exp:
	"identifier" "(" params ")"	{ $$ = new top::funccall_node($1, $3); }
|	exp "+" exp					{ $$ = new top::binop_node($1, "+", $3); }
| 	exp "-" exp					{ $$ = new top::binop_node($1, "-", $3); }
| 	exp "*" exp					{ $$ = new top::binop_node($1, "*", $3); }
| 	exp "/" exp					{ $$ = new top::binop_node($1, "/", $3); }
|	"-" exp %prec UMINUS		{ $$ = new top::unop_node("-", $2); } /* 4 SR */
| 	"(" exp ")"					{ $$ = $2; }

|   exp "->" exp									{ std::cout << "rcv" << std::endl; }
|	exp "->" "tidentifier"							{ std::cout << "obj " << $3 << std::endl; }
|   "tidentifier" "->" "identifier" "(" params ")"	{ std::cout << "obj " << $1 << " func " << $3 << std::endl; }
|	"tidentifier" "->" "tidentifier"				{ std::cout << "obj " << $1 << " obj " << $3 << std::endl; }
| 	"tidentifier"				{ $$ = new top::variable_node($1); } /* 2 SR */
| 	"int"						{ $$ = new top::int_node($1); }
|	"number"					{ $$ = new top::float_node($1); }
|	"stringliteral"				{ $$ = new top::string_node($1); }


params:
	%empty			{ $$ = new top::params_node(); }
| 	exp				{ $$ = new top::params_node(); $$->add($1); }
| 	exp "," params	{ $3->add($1); $$ = $3; }


%%

void yy::top_parser::error(const location_type& l, const std::string& m) {
	driver.error (l, m);
}
