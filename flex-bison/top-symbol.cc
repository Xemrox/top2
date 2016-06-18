#include "top-symbol.hh"

const std::map<top::symbol::Type, const std::string> top::symbol::trans
{
	{ Type::PKG, "PKG" },
	{ Type::FUNC, "FUNC" },
	{ Type::VAR, "VAR" },
};