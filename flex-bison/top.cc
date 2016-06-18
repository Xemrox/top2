/*
CC-SS16
GRABOSCH & WALTER
*/
#include <iostream>
#include <stdexcept>
#include "top-driver.hh"
#include "ArgumentHandler.h"
#include "Logging.h"

using std::endl;
using std::exception;
using std::runtime_error;

using namespace go;

int main (int argc, char *argv[]) {
	//try {
		ARG.ParseArgs(argc, argv);
			//ARG.Pipes[PT::IN]->_File = "var-main;/var-main;.top";
			//ARG.Pipes[PT::IN]->used = false;

		if (ARG.Test()) {
			ARG.TestArgs();
		}
		if (!ARG[PT::CWD]->used) {
			//CHDIR(ARG[PT::CWD]->File().c_str());
		}

		int res = 0;
		top_driver driver;
		for (int i = 1; i < argc; ++i) {
			if (argv[i] == std::string ("-p")) {
				driver.trace_parsing = true;
			} else if (argv[i] == std::string ("-s")) {
				driver.trace_scanning = true;
			} else if (!driver.parse(argv[i])) {
				ARG.OpenPipes();

				std::cout << driver.result << std::endl;
				std::cout << "Package: " << driver.package << std::endl;
				if (driver.imports.size() > 0U) {
					std::cout << "Imports: " << driver.imports.at(0U);
					for (unsigned int i(1U); i < driver.imports.size(); ++i)
				 		std::cout << ", " << driver.imports.at(i);
					std::cout << std::endl;
				}
				driver.body->print(std::cout);

				for (auto e : driver.symbols) {
					std::cout << "# " << top::symbol::tostring(e.type) << std::endl;
					std::cout << "Token";
					e.token->print(std::cout, 1, true);
					std::cout << "Scope";
					if (e.scope != NULL) {
						e.scope->print(std::cout, 1, true);
					} else {
						std::cout << "  -> GLOBAL" << std::endl;
					}
					std::cout << std::endl;
				}

				ARG.ClosePipes();
			} else {
				res = 1;
			}
		}
		if(driver.hasError)
			return 1;
		return res;
	/*} catch (runtime_error &err) {
		log::ERR << "Runtime error: " << err.what() << endl;
		return 2;
	} catch (exception &ex) {
		log::ERR << "Exception: " << ex.what() << endl;
		return 1;
	} catch (...) {
		log::ERR << "Evil things" << endl;
		return -1;
	}*/
}
