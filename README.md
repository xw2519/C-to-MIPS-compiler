# C-To-MIPS Compiler

A C to MIPS compiler based on the ANSI C specification and developed in C++. 

This project specifications is based on ELEC50010: Instruction Architecture and Compilers [`https://github.com/LangProc/langproc-2019-cw.git`].

# Table of contents
- [C-To-MIPS Compiler](#c-to-mips-compiler)
- [Table of contents](#table-of-contents)
- [Deliverables](#deliverables)
  - [Compiler](#compiler)
    - [Building](#building)
  - [Testbench](#testbench)
    - [Building](#building-1)
- [Contributors](#contributors)

# Deliverables

The deliverables consists of the following:
- `compiler.cpp`: Compiles a given ANSI C program into MIPS assembly language.
- `test_compiler.sh`: A testbench for the compiler that uses the test cases in the `compiler_tests` folder and compares the generated MIPS assembly language with MIPS-QEMU. 

## Compiler

The development of the compiler was inspired based on the ANSI C Yacc Grammer published by Jeff Lee [`https://www.lysator.liu.se/c/ANSI-C-grammar-y.html`].  

The compiler is composed of two main file: lexer and parser. The compiler file dependencies is shown below:
```
compiler.cpp
|-- lexer.flex
|-- parser.y
    |-- ast_context.hpp
    |-- ast_declaration.hpp
    |-- ast_expression.hpp
    |-- ast_primitive.hpp
    |-- ast_statement.hpp
```
### Building
The compiler is compiled through the `Makefile` by calling the following commands through the Linux terminal:
```
make clean 
make all
```

## Testbench
There are three types of testbenches written in Linux bash:
- `test_testcase.sh`
- `test_folder.sh`
- `test_compiler.sh`

### Building
Each testbench script can be executed through the Linux terminal such as:
```
./test_testcase.sh
```



# Contributors

- Ebby Samson: ebby.samson19@imperial.ac.uk
- Xin Wang: xin.wang19@imperial.ac.uk

