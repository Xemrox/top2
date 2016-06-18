#include "Logging.h"

//GENERAL PURPOSE
std::ostream go::log::LOG(std::cout.rdbuf());
std::ostream go::log::OUT(std::cout.rdbuf());
std::istream go::log::IN(std::cin.rdbuf());
std::ostream go::log::ERR(std::cerr.rdbuf());

//SPECIAL CASE
std::ostream go::log::LEX(std::cout.rdbuf());
std::ostream go::log::PRS(std::cout.rdbuf());
std::ostream go::log::AST(std::cout.rdbuf());
std::ostream go::log::SYM(std::cout.rdbuf());