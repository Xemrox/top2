#include "top-driver.hh"
#include "top-parser.hh"

top_driver::top_driver ()
  : trace_scanning (false), trace_parsing (false), body(new top::body_node()), hasError(false) {
}

top_driver::~top_driver () { }

int top_driver::parse (const std::string &f) {
	file = f;
	scan_begin();
	yy::top_parser parser(*this);
	parser.set_debug_level(trace_parsing);
	int res = parser.parse();
	scan_end();
	return res;
}

void top_driver::error (const yy::location& l, const std::string& m) {
	this->hasError = true;
	std::cerr << l << ": " << m << std::endl;
}

void top_driver::error (const std::string& m) {
	this->hasError = true;
	std::cerr << m << std::endl;
}
