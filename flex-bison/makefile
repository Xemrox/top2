####################################################################
# This Makefile started out as a copy of the one in the flex manual.
# Cf. http://flex.sourceforge.net/manual/Makefiles-and-Flex.html#Makefiles-and-Flex
#
# It replaces the amazingly complex Makefile that comes with the C++calc example
# found in the Bison manual.
#
# This is Verison 0.2 of the Makefile (as of 17 April 2013, 22:00
# The previous (unnumbered) version only worked with MAKEFLAGS=-j3
# (and it's strange that it worked then).
#
####################################################################
#      "Variables", die hier benutzt werden:
#      Vgl. http://www.makelinux.net/make3/make3-CHP-2-SECT-2.html
# $@ = The filename representing the target.
# $< = The filename of the first prerequisite.
# $* = The stem of the filename of the target (i.e. without .o, .cpp...)
# $^ = The names of all the prerequisites, with spaces between them.
####################################################################
# Uncomment only one of the next two lines (choose your c++ compiler)
# CC=g++
CC=clang++
CFLAGS=-std=c++11 -Wno-deprecated-register

LEX=flex
YACC=bison
YFLAGS=-v -d -Wconflicts-sr -Wconflicts-rr
# Wo   -d wichtig ist, weil damit Header-Dateien erzeugt werden
#         (*.hh - und nicht nur Quellcode in *.cc)
# aber -v nicht so wichtig ist, weil damit "nur" die  Datei
#         go-parser.output erzeugt wird, die zwar informativ aber nicht
#         unbedingt notwendig (sie wird nicht weiterverwendet).

default: top

top.o : top.cc top-driver.hh top-parser.hh top-node.hh top-symbol.hh ArgumentHandler.h Logging.h

top: top.o top-scanner.o top-parser.o top-driver.o top-node.o top-context.o top-symbol.o ArgumentHandler.o Logging.o
	$(CC) $(CFLAGS) $^ -o $@ `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit lto native` -fexceptions
	
%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< `llvm-config --cxxflags` -fexceptions

%.o: %.cc
	$(CC) $(CFLAGS) -o $@ -c $< `llvm-config --cxxflags` -fexceptions

top-scanner.cc: top-scanner.ll
	$(LEX) $(LFLAGS) -o top-scanner.cc top-scanner.ll

top-parser.cc top-parser.hh: top-parser.yy
	$(YACC) $(YFLAGS) -o top-parser.cc top-parser.yy

clean:
	$(RM) *~ *.o  top  top-scanner.cc top-parser.cc top-scanner.hh top-parser.hh top-parser.output location.hh stack.hh position.hh

