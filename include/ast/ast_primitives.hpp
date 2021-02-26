#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include "ast_node.hpp"

#include <string>

class String_Literal : public Node
{

private:
	std::string str;
public:
	String_Literal(std::string _str):str(_str){}

	virtual void print_C(std::ostream &dst, std::string indent) const override
	{
		dst << str;
	}

};

#endif