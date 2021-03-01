#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include "ast_node.hpp"

#include <iomanip>
#include <string>

/* -------------------------------- 			      Base class		 			-------------------------------- */

class Primitive : public Expression {};

/* -------------------------------- 				Derivative class		 		-------------------------------- */

class Constant : public Primitive
{
	private:
		int value;

	public:
		Constant(int _value) : value(_value) {}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{

		}
};

class Identifier : public Primitive
{
	private:
		std::string ID;

	public:
		Identifier(std::string _ID) : ID(_ID) {}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{

		}
};

class StringLiteral : public Primitive{
	private:
		std::string str;
	public:
		StringLiteral(std::string _str):str(_str){}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override{
			dst << str;
		}
};


#endif