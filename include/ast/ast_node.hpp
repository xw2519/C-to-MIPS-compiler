#ifndef NODE
#define NODE

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <regex>
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <initializer_list>

class Context;
class Enumerator;
class StructDeclaration;
class Expression;
class Statement;
class Declaration;
class PrimitiveType;

enum ExpressionEnum { DOT, ARROW, INCREMENT, DECREMENT, REFERENCE, DEREFERENCE, BITWISE_AND, BITWISE_OR, BITWISE_XOR,
                      PLUS, MINUS, BITWISE_NOT, LOGICAL_NOT, SIZEOF, MULTIPLY, DIVIDE, MODULO, SHIFT_LEFT, SHIFT_RIGHT,
                      LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
                      DIV_ASSIGN, MOD_ASSIGN, SHIFT_LEFT_ASSIGN, SHIFT_RIGHT_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN,
                      INT, CHAR, UCHAR, UNSIGNED, FLOAT, DOUBLE, STRING_LITERAL, STRUCT, LOGICAL_AND, LOGICAL_OR };

enum StatementEnum { CASE, DEFAULT, EXPRESSION, IF, ELSE, SWITCH, DO, WHILE, FOR, CONTINUE, BREAK, RETURN, COMPOUND, EXPR_STMT };

enum DeclarationEnum { D_CHAR, D_UCHAR, D_INT, D_UNSIGNED, D_FLOAT, D_DOUBLE, TYPEDEF, TYPEID, STRUCTS, ENUMS };

class Node
{
	public:


		virtual ~Node()
		{}

		virtual void print_mips (std::ostream &dst, Context& context) const
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};


#endif
