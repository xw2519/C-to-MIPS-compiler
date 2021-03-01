#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include "ast_node.hpp"

#include <string>
#include <iostream>
#include <map>
#include <memory>

class Expression;

typedef const Expression *Expression_Ptr;

/* -------------------------------- Expression Base Class -------------------------------- */

class Expression : public Node {};


/* -------------------------------- Unary Expression -------------------------------- */

class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression(Expression* _expression) : expression(_expression) {}
};


/* -------------------------------- Post-fix Expression -------------------------------- */

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression(Expression* _expression) : Unary_Expression(_expression) {}
};


class Function_Call_Expression : public Unary_Expression{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression(Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression(_expression), argument_list(_argument_list) {}

};

/* -------------------------------- Assignment Expression -------------------------------- */

class Assignment_Expression : public Expression // Base class
{
	protected:
		Expression* lvalue;
		Expression* expression;
			
	public:
		Assignment_Expression(Expression* _lvalue, Expression* _expression) : lvalue(_lvalue), expression(_expression) {}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment(Expression* _lvalue, Expression* _expr) : Assignment_Expression(_lvalue, _expr) {}
};

/* -------------------------------- Operator Expression -------------------------------- */

class Operator : public Expression
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Operator(Expression* _left, Expression* _right) : left(_left), right(_right) {}
};

/* -------------------------------- Arithmetic Expression -------------------------------- */

class Add_Expression : public Operator
{
	public:
		Add_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void print_MIPS(std::ostream &dst, Context& context) const override
		{

		}
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void print_MIPS(std::ostream &dst, Context& context) const override
		{

		}
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}


};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}

};



/* -------------------------------- Relational Binary Expressions -------------------------------- */

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};



#endif