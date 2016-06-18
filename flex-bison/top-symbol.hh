#pragma once

#include <map>
#include <string>
#include "top-node.hh"

namespace top {

	class symbol {
	public:
		enum class Type {
			VAR,
			FUNC,
			PKG
		};

	private:
		static const std::map<Type, const std::string> trans;

	public:
		node* token;
		node* scope;

		Type type;

		symbol(node* token, node* scope, Type type) :
			token(token), scope(scope), type(type) {};

		const static std::string& tostring(Type type) {
			return trans.at(type);
		};
	};
}