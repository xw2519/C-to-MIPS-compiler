  
CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I inc

all : bin/c_compiler

src/lexer.yy.cpp:	src/lexer.flex src/parser.tab.hpp
	echo "This line will only print once"
	flex -o src/lexer.yy.cpp  src/lexer.flex

src/parser.tab.cpp src/parser.tab.hpp:	src/parser.y
	bison -v -d src/parser.y -o src/parser.tab.cpp

bin/c_compiler:	src/print_canonical.o src/parser.tab.o src/lexer.yy.o
	echo "This line will only print once"
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

clean :
	rm src/*.yy.cpp
	rm src/*.tab.hpp
	rm src/*.output
	rm src/*.o
	rm bin/*
	