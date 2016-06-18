/***********************
 * Example of C++ in Bison (yacc)
 * Compare Bison Manual, Section 10.1.6 A Complete C++ Example
 * https://www.gnu.org/software/bison/manual/html_node/A-Complete-C_002b_002b-Example.html
 *
 * This is the (f)lex file, i.e. token definitions
 * See also top-parser.yy for the parser (grammar)
 * (yacc/bison input).
 *
 ***********************/

%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <string>
#include "top-driver.hh"
#include "top-parser.hh"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.	See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
#undef yywrap
#define yywrap() 1

// The location of the current token.
static yy::location loc;
%}
%option noyywrap nounput batch debug noinput
stringliteral   \".*\"
tid			[sidfbo][A-Z_][a-zA-Z0-9_]*
id			[a-zA-Z_][a-zA-Z0-9_]*
number		-?([1-9][0-9]+|[0-9])(\.[0-9]*)?
int			-?([1-9][0-9]+|[0-9])?
blank		 [ \t\r;]

%{
	// Code run each time a pattern is matched.
	#define YY_USER_ACTION	loc.columns (yyleng);
%}

%s IN_COMMENT

%%

%{
	// Code run each time yylex is called.
	loc.step ();
%}

<INITIAL>{
"/*"        BEGIN(IN_COMMENT);
{blank}+	loc.step ();
[\n]+		loc.lines (yyleng); loc.step ();
"-"	        return yy::top_parser::make_MINUS(loc);
"+"         return yy::top_parser::make_PLUS(loc);
"*"		    return yy::top_parser::make_STAR(loc);
"/"         return yy::top_parser::make_SLASH(loc);
"("         return yy::top_parser::make_LPAREN(loc);
")"         return yy::top_parser::make_RPAREN(loc);
"{"         return yy::top_parser::make_LBLOCK(loc);
"}"         return yy::top_parser::make_RBLOCK(loc);
"="         return yy::top_parser::make_ASSIGN(loc);
","         return yy::top_parser::make_COMMA(loc);
"->"		return yy::top_parser::make_ARROW(loc);
"function"  return yy::top_parser::make_FUNCTION(loc);
"enum"		return yy::top_parser::make_ENUM(loc);
"obj"		return yy::top_parser::make_OBJ(loc);
"if"		return yy::top_parser::make_IF(loc);
"while"		return yy::top_parser::make_WHILE(loc);
"end"		return yy::top_parser::make_ENDSTUFF(loc);
"for"		return yy::top_parser::make_FOR(loc);
"do"		return yy::top_parser::make_DO(loc);
"var"       return yy::top_parser::make_VAR(loc);
"return"    return yy::top_parser::make_RETURN(loc);
"new"		return yy::top_parser::make_NEW(loc);
"parameters" return yy::top_parser::make_PARAMETERS(loc);
"//".*      { }
}
<IN_COMMENT>{
"*/"        BEGIN(INITIAL);
[^*\n]+     { }
"*"         { }
"\n"        yylineno++;
}

{stringliteral} return yy::top_parser::make_STRING(yytext, loc);

{int} {
	errno = 0;
	long n = strtol (yytext, NULL, 10);
	if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
		driver.error (loc, "integer is out of range");
	return yy::top_parser::make_INT(n, loc);
}

{number} {
	errno = 0;
	double n = strtof(yytext, NULL);
	if (! (DBL_MIN <= n && n <= DBL_MAX && errno != ERANGE))
		driver.error (loc, "double is out of range");
	return yy::top_parser::make_NUMBER(n, loc);
}

{tid}		return yy::top_parser::make_TIDENTIFIER(yytext, loc);

{id}		return yy::top_parser::make_IDENTIFIER(yytext, loc);

.			driver.error (loc, "invalid character '" + std::string(yytext) + "'");

<<EOF>>		return yy::top_parser::make_END(loc);

%%

void top_driver::scan_begin () {
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-")
	  yyin = stdin;
	else if (!(yyin = fopen (file.c_str (), "r"))) {
		error ("cannot open " + file + ": " + strerror(errno));
		exit (EXIT_FAILURE);
	}
}

void top_driver::scan_end () {
	fclose (yyin);
}
