#pragma once

#include <iostream>
#include <stdexcept>

#include "ArgumentHandler.h"

//different log classes and levels ready for redirect etc
namespace go {
	namespace log {
		//input sink for unused output
		class NullBuffer : public std::streambuf {
		public:
			int overflow(int c) { return c; }
		};
		class NullStream : public std::ostream {
		public:
			NullStream() : std::ostream(&m_sb) {}
		private:
			NullBuffer m_sb;
		};

		//first they are global renames
		//but after applyRedirects they get their own variants for files / pipes

		//GENERAL PURPOSE
		extern std::ostream LOG;
		extern std::ostream OUT;
		extern std::istream IN;
		extern std::ostream ERR;

		//SPECIAL CASE
		extern std::ostream LEX;
		extern std::ostream PRS;
		extern std::ostream AST;
		extern std::ostream SYM;
	}
}