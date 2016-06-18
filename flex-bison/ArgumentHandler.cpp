#include "ArgumentHandler.h"

namespace go {
	namespace util {
		using std::string;
		using std::vector;
		using std::map;
		using std::any_of;
		using std::regex;
		using std::regex_match;

		using std::cout;
		using std::endl;
		using std::runtime_error;

		vector<ArgumentHandler::Pipe> ArgumentHandler::vPipes{
			ArgumentHandler::Pipe(PipeType::CWD, "CWD", "cwd"),
			ArgumentHandler::Pipe(PipeType::IN, "INPUT", "i|in|ifile|input", &go::log::IN),
			ArgumentHandler::Pipe(PipeType::OUT, "OUTPUT", "o|out|ofile|output", &go::log::OUT),
			ArgumentHandler::Pipe(PipeType::ERR, "ERROR", "e|err|errfile", &go::log::ERR),
			ArgumentHandler::Pipe(PipeType::LEX, "LEXER", "l|lex|lfile", &go::log::LEX),
			ArgumentHandler::Pipe(PipeType::PST, "PARSER", "p|prs|pfile", &go::log::PRS),
			ArgumentHandler::Pipe(PipeType::AST, "AST", "a|ast|afile", &go::log::AST),
			ArgumentHandler::Pipe(PipeType::SYM, "SYM", "s|sym|sfile", &go::log::SYM),
		};

		ArgumentHandler::ArgumentHandler() : _Test(false), Pipes() {
			for (vector<ArgumentHandler::Pipe>::iterator i = vPipes.begin(); i != vPipes.end(); i++) {
				this->Pipes[(*i).ID] = &(*i);
			}
		}

		void ArgumentHandler::ParseArgs(int argc, char * argv[]) {
			vector<std::string> vArgs(argv, argv + argc);
			//vector<string> -> string
			string sArgs = std::accumulate(vArgs.begin(), vArgs.end(), string(),
											[](string &ss, string &s) {
				return ss.empty() ? s : ss + " " + s;
			});

			this->_Test = any_of(vArgs.begin(), vArgs.end(),
									[](string str) {
				return str.compare(2, string::npos, "test") == 0;
			});

			auto parsePip = [sArgs](string const words, string &val) -> bool {
				std::string rgx = ".*\\s+-{1,2}(" + words + ")=([\\w.\\/\\\\;\\-_]+)\\s*.*";
				regex rPip(rgx, std::regex_constants::icase);
				std::smatch smMatch;
				if (regex_match(sArgs, smMatch, rPip, std::regex_constants::match_any) && smMatch.size() > 1) {
					val = smMatch[2]; //second capture group
				} else {
					return true;
				}
				return false;
			};

			for (ArgumentHandler::Pipe &pipe : this->vPipes) {
				pipe.used = parsePip(pipe.EX, pipe._File);
			}
		}

		void ArgumentHandler::TestArgs() const {
			cout << "Printing args:" << endl;

			for (auto pipe : this->Pipes) {
				cout << '\t';
				cout << pipe.second->NAME << ": ";
				if (!pipe.second->used) {
					cout << pipe.second->File();
				} else {
					cout << "pipe";
				}
				cout << endl;
			}
		}

		void ArgumentHandler::Pipe::RedirectStreams() {
			if (this->used) return;

			if (this->INSTREAM != 0) {
				this->INFSTREAM = new std::ifstream(this->File().c_str());

				if (!this->INFSTREAM->is_open())
					throw std::runtime_error("could not open input file");

				//copy whole streamstate to new stream
				this->INBUFF = this->INSTREAM->rdbuf(this->INFSTREAM->rdbuf());
				this->INSTREAM->copyfmt(*this->INFSTREAM);
				this->INSTREAM->clear(this->INFSTREAM->rdstate());
			} else if (this->OUTSTREAM != 0) {
				this->OUTFSTREAM = new std::ofstream(this->File().c_str());

				if (!this->OUTFSTREAM->is_open())
					throw std::runtime_error("could not open output file");

				//copy whole streamstate to new stream
				this->OUTBUFF = this->OUTSTREAM->rdbuf(this->OUTFSTREAM->rdbuf());
				this->OUTSTREAM->copyfmt(*this->OUTFSTREAM);
				this->OUTSTREAM->clear(this->OUTFSTREAM->rdstate());
			}
		}

		void ArgumentHandler::Pipe::FinishStreams() {
			if (this->used) return;
			//either we got an input or an output stream
			if (this->INFSTREAM != 0 && this->INFSTREAM->is_open()) {
				//reset stream state
				this->INSTREAM->rdbuf(this->INBUFF);

				this->INFSTREAM->close();

				delete this->INFSTREAM;
				this->INFSTREAM = 0;
			} else if (this->OUTFSTREAM != 0 && this->OUTFSTREAM->is_open()) {
				//reset stream state
				this->OUTSTREAM->rdbuf(this->OUTBUFF);

				OUTFSTREAM->flush();
				OUTFSTREAM->close();

				delete this->OUTFSTREAM;
				this->OUTFSTREAM = 0;
			}
		}

		void ArgumentHandler::OpenPipes() {
			for (auto &kv : this->Pipes) {
				auto *pipe = kv.second;
				if (!pipe->used && pipe->ID != PipeType::CWD) {
					if (this->Test())
						std::cout << "Redirecting " << pipe->NAME << " to file: " << pipe->File() << endl;
					pipe->RedirectStreams();
				}
			}
		}

		void ArgumentHandler::ClosePipes() {
			for (auto &kv : this->Pipes) {
				auto *pipe = kv.second;
				if (!pipe->used && pipe->ID != PipeType::CWD) {
					if (this->Test())
						std::cout << "Redirecting " << pipe->NAME << " back" << endl;
					pipe->FinishStreams();
				}
			}
		}

		ArgumentHandler ARG;
	}
}