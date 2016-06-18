#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>

#include <numeric>
#include <stdexcept>
#include <map>

#include "Logging.h"

namespace go {
	namespace util {
		class ArgumentHandler {
		public:
			class Pipe;
			enum class PipeType {
				//hack
				CWD,

				//
				IN,
				OUT,
				ERR,

				LEX,
				PST,
				AST,
				SYM
			};
		private:
			bool _Test;

			ArgumentHandler(ArgumentHandler &copy) {};

			static std::vector<Pipe> vPipes;
		public:
			class Pipe {
			public:
				std::streambuf *INBUFF;
				std::streambuf *OUTBUFF;

				std::istream *INSTREAM;
				std::ostream *OUTSTREAM;

				std::ifstream *INFSTREAM;
				std::ofstream *OUTFSTREAM;

				const PipeType ID;
				const std::string NAME;
				const std::string EX;
				bool used = true;
				std::string _File;

				std::string File() const {
					if (this->_File.empty() && !this->used)
						throw std::runtime_error("file mode needs file(" + this->NAME + ")");
					return _File;
				}

				Pipe(PipeType _ID, std::string _NAME, std::string _EX) :
					INSTREAM(0), OUTSTREAM(0),
					INFSTREAM(0), OUTFSTREAM(0),
					ID(_ID), NAME(_NAME), EX(_EX),
					used(false), _File("") {};

				Pipe(PipeType _ID, std::string _NAME, std::string _EX, std::istream *_STREAM) :
					INSTREAM(_STREAM), OUTSTREAM(0),
					INFSTREAM(0), OUTFSTREAM(0),
					ID(_ID), NAME(_NAME), EX(_EX),
					used(false), _File("") {};
				Pipe(PipeType _ID, std::string _NAME, std::string _EX, std::ostream *_STREAM) :
					INSTREAM(0), OUTSTREAM(_STREAM),
					INFSTREAM(0), OUTFSTREAM(0),
					ID(_ID), NAME(_NAME), EX(_EX),
					used(false), _File("") {};

				void RedirectStreams();

				void FinishStreams();

				~Pipe() {
					this->FinishStreams();
				};
			};

			ArgumentHandler();
			~ArgumentHandler() {};

			Pipe* operator[](PipeType ID) {
				return this->Pipes[ID];
			};

			std::map<PipeType, Pipe*> Pipes;

			void ParseArgs(int argc, char* argv[]);
			void TestArgs() const;

			void OpenPipes();
			void ClosePipes();


			bool Test() const {
				return this->_Test;
			}
		};

		extern ArgumentHandler ARG;
	}
}
using go::util::ARG;
typedef go::util::ArgumentHandler::PipeType PT;
