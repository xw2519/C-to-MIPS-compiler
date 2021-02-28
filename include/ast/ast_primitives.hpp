#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include "ast_node.hpp"

#include <iomanip>
#include <string>


class Primitive : public Expression
{

};


class String_Literal : public Primitive
{
	private:
		std::string str;
	public:
		String_Literal(std::string _string):str(_string) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << str;
		}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			dst << indent << str;
		}
};

class Constant : public Primitive
{
	private:
		int value;

	public:
		Constant(int _value) : value(_value) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << value;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << indent << value;
		}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{
			std::string destReg = ctx.get_dest_reg();
			dst << "addiu $" << destReg << ",$0," << value << std::endl;
		}
};

class Identifier : public Primitive
{
	private:
		std::string ID;

	public:
		Identifier(std::string _ID) : ID(_ID) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << ID;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << indent << ID;
		}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{
			std::string destReg = ctx.get_dest_reg();

			//std::cout << "addiu $"<<destReg<<",$0,"<<val<<std::endl;
		}
};


#endif