#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP

#include "ast_expression.hpp"

class Statement : public Node
{
protected:
	Statement *next;
	Statement(Statement *_next = NULL):next(_next){} 
};

class CompoundStatement : public Statement
{
	private:

	public:
		CompoundStatement( Statement* _s_list = NULL ){}

		virtual void print_structure(std::ostream &dst) const override
        {
		 	// To be done later
		}
};

class JumpStatement : public Statement
{
	private:
		std::string str;

	public:

		JumpStatement( std::string _str, Statement *_next = NULL):Statement(_next),str(_str)
        {
			// To be done later
		}

		virtual void print_structure(std::ostream &dst) const override
        {
			// To be done later
		}

};

#endif