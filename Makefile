
CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I inc


all : src/parser.tab.cpp src/parser.tab.hpp src/lexer.yy.cpp bin/c_compiler compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	@echo "Compiling parser"
	bison -v -d src/parser.y -o src/parser.tab.cpp


src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	@echo "Compiling lexer"
	flex -o src/lexer.yy.cpp  src/lexer.flex


bin/c_compiler : src/compiler.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	@echo "Compiling compiler"
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^


compiler : bin/c_compiler


clean :
	rm src/*.o
	rm bin/*
	rm src/*.tab.cpp
	rm src/*.tab.hpp
	rm src/*.yy.cpp
	rm src/*.output
