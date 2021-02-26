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
    virtual ~Expression() {}

    // Evaluate tree using the given mapping of variables to numbers
    virtual int evaluate( const std::map<std::string,double> &bindings) const
    { 
		throw std::runtime_error("Expression 'evaluate' not implemented."); 
	}
};

/* -------------------------------- Assignment Expression -------------------------------- */

class Assignment : public Expression // Base class
{
	protected:
		Expression* lvalue;
		Expression* expr;
			
	public:
		Assignment(Expression* _lvalue, Expression* _expr) : lvalue(_lvalue), expr(_expr) {}
};

class Direct_Assignment : public Assignment{

	public:
		Direct_Assignment(Expression* _lvalue, Expression* _expr) : Assignment(_lvalue, _expr) {}

		virtual void print_structure (std::ostream &dst) const override
		{
			dst << " Expression: Direct Assignment " << std::endl;
		}
		
		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			lvalue->print_C(dst, indent);
			dst << " =";
			expr->print_C(dst," ");
		}
};

/* -------------------------------- Binary Expression -------------------------------- */

class Binary_Expression : public Expression{
	protected:
		Expression* left;
		Expression* right;

	public:
		Binary_Expression(Expression* _left, Expression* _right) : left(_left), right(_right) {}

		virtual const char op() const =0;

		virtual void print_C(std::ostream &dst,std::string indent) const override{
			dst << "(";
			left->print_C(dst, indent);
			dst << op();
			right->print_C(dst, "");
			dst << ")";
		}

		virtual void print_structure(std::ostream &dst) const override
		{
			dst << "( ";
	        left->print_structure(dst);
	        dst << " ";
	        dst << op();
	        dst << " ";
	        right->print_structure(dst);
	        dst << " )";
		}
};



#endif