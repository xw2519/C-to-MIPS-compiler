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

class Expression : public Node
{
	public:
		virtual void print_structure(std::ostream &dst, int m) const =0;
};


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

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			expression->print_C(dst, indent);
			dst<< "++";
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			expression->print_structure(dst, m);
			dst << "++" << std::endl;
		}
};


class Function_Call_Expression : public Unary_Expression{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression(Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression(_expression), argument_list(_argument_list) {}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			expression->print_C(dst, indent);
			dst << "(";
			if(argument_list != NULL)
			{
				for(auto it=argument_list->begin(); it != argument_list->end(); it++){
					(*it)->print_C(dst,"");
					if(next(it,1)!=argument_list->end()){
						dst << ",";
					}
				}
			}
			dst << ")";
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << "Function Call Expression [ Identifier ( ";  expression->print_structure(dst, m); dst << " ) ";
			if(argument_list->size() != 0)
			{
				for(auto it = argument_list->begin(); it != argument_list->end(); it++)
				{
					(*it)->print_structure(dst, m);
				}
			}
			dst << "]" << std::endl;
		}

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

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << " Expression: Direct Assignment " << std::endl;
		}
		
		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			lvalue->print_C(dst, indent);
			dst << " =";
			expression->print_C(dst," ");
		}
};

/* -------------------------------- Binary Expression -------------------------------- */

class Binary_Expression : public Expression
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Binary_Expression(Expression* _left, Expression* _right) : left(_left), right(_right) {}

		virtual const char *getOp() const =0;

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << "(";
			left->print_C(dst,indent);
			dst << getOp();
			right->print_C(dst,"");
			dst <<")";
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst<<"( ";
	        left->print_structure(dst,m);
	        dst<<" ";
	        dst<<getOp();
	        dst<<" ";
	        right->print_structure(dst,m);
	        dst<<" )";
		}
};

/* -------------------------------- Arithmetic Expression -------------------------------- */

class Multiply_Expression : public Binary_Expression
{
	public:
		Multiply_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}

		virtual const char *getOp() const override
		{
			return "*";
		}
};

class Divide_Expression : public Binary_Expression
{
	public:
		Divide_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}

		virtual const char *getOp() const override
		{
			return "/";
		}
};

class Add_Expression : public Binary_Expression
{
	public:
		Add_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left, _right) {}

		virtual const char *getOp() const override
		{
			return "+";
		}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{
			std::string destReg = ctx.get_dest_reg();
			left->print_MIPS(dst,ctx);
			std::string tempReg = ctx.alloc_free_reg();
			right->print_MIPS(dst,ctx);
			dst << "addu $"<<destReg<<",$"<<destReg<<",$"<<tempReg<<std::endl;
		}
};

class Sub_Expression : public Binary_Expression
{
	public:
		Sub_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}

		virtual const char *getOp() const override
		{
			return "-";
		}

		virtual void print_MIPS(std::ostream &dst, Context& ctx) const override
		{
			std::string destReg = ctx.get_dest_reg();
			left->print_MIPS(dst, ctx);
			std::string tempReg = ctx.alloc_free_reg();
			right->print_MIPS(dst, ctx);
			dst << "subu $"<<destReg<<",$"<<destReg<<",$"<<tempReg<<std::endl;
		}	
};


/* -------------------------------- Relational Binary Expressions -------------------------------- */

class Less_Than_Expression : public Binary_Expression
{
	public:
		Less_Than_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left, _right) {}

		virtual const char *getOp() const override
		{
			return "<";
		}
};

class More_Than_Expression : public Binary_Expression
{
	public:
		More_Than_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left, _right) {}

		virtual const char *getOp() const override
		{
			return ">";
		}
};

class Less_Than_Equal_Expression : public Binary_Expression
{
	public:
		Less_Than_Equal_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left, _right) {}

		virtual const char *getOp() const override
		{
			return "<=";
		}
};

class More_Than_Equal_Expression : public Binary_Expression
{
	public:
		More_Than_Equal_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}
 
		virtual const char *getOp() const override
		{
			return ">=";
		}
};

class Equal_Expression : public Binary_Expression
{
	public:
		Equal_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}

		virtual const char *getOp() const override
		{
			return "==";
		}
};

class Not_Equal_Expression : public Binary_Expression
{
	public:
		Not_Equal_Expression(Expression* _left, Expression* _right) : Binary_Expression(_left,_right) {}

		virtual const char *getOp() const override
		{
			return "!=";
		}
};



#endif