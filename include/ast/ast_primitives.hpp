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
		Constant (int _value) : value (_value) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			dst << "\t" << "li" << "\t" << "$" << "v0" << ", " << value << std::endl;
		}
};

class Variable : public Primitive // Local variables with constant
{
	private:
		std::string ID;

	public:
		Variable (std::string _ID) : ID (_ID) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{	
			// Allocate memory
			std::vector<int> temp_registers = context.list_available_return_registers(); 
			context.set_unavaiable(temp_registers[0]);

			// dst << "\t" << "lw" << "\t" << "$" << temp_registers[0] << ", " << "To be implemented" << "($fp)" <<std::endl;
		}
};

class StringLiteral : public Primitive
{
	private:
		std::string str;
	public:
		StringLiteral (std::string _str) : str (_str) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			dst << str;
		}
};


#endif