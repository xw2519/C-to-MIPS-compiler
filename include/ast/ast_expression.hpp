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
		Unary_Expression (Expression* _expression) : expression(_expression) {}
};


/* -------------------------------- Post-fix Expression -------------------------------- */

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression (Expression* _expression) : Unary_Expression(_expression) {}
};


class Function_Call_Expression : public Unary_Expression{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression (Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression (_expression), argument_list (_argument_list) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			dst << " Function_Call_Expression " << std::endl;
		}

};

/* -------------------------------- Assignment Expressions -------------------------------- */

class Assignment_Expression : public Expression // Base class
{
	protected:
		Expression* left_value;
		Expression* expression;
			
	public:
		Assignment_Expression (Expression* _left_value, Expression* _expression) : left_value (_left_value), expression (_expression) {}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment (Expression* _left_value, Expression* _expression) : Assignment_Expression (_left_value, _expression) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			// Track destination variable location
			int destination_frame_pointer = context.get_frame_pointer();

			// Execute left value 
			left_value->print_MIPS(dst, context);	

			// Allocate stack
			std::vector<int> temp_registers = context.list_available_temprorary_registers();
			context.set_unavaiable(temp_registers[0]);

			context.decrease_frame_pointer();

			
			// Execute expressions on right hand side
			expression->print_MIPS(dst, context);

			// Print assembly
			dst << "\t" << "sw" << "\t" << "$" << "v0" << ", " << 8 << "($fp)" <<std::endl;

			// Deallocate stack
			context.increase_frame_pointer();

			context.set_avaiable(temp_registers[0]);
		}
};

/* -------------------------------- Operator Expression -------------------------------- */

class Operator : public Expression
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Operator (Expression* _left, Expression* _right) : left (_left), right (_right) {}
};

/* -------------------------------- Arithmetic Expression -------------------------------- */

class Add_Expression : public Operator
{
	public:
		Add_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			// Allocate memory




			// Deallocate memory
		}
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			// Allocate memory
			std::vector<int> temp_registers = context.list_available_temprorary_registers(); 
			context.set_unavaiable(temp_registers[0]);

			// Execute branches
			left->print_MIPS(dst, context);
            right->print_MIPS(dst, context);

			dst << "\t" << "sub" << "\t" << "$" << "v0" << ", $" << "v0" << ", $" << temp_registers[0] << std::endl;

			// Deallocate memory
			context.set_avaiable(temp_registers[0]);
		}
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}


};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

};



/* -------------------------------- Relational Binary Expressions -------------------------------- */

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}
};



#endif