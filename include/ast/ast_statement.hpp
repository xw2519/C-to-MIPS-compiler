#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP

#include "ast_expression.hpp"
#include "ast_declaration.hpp"

#include <iomanip>

class Expression_Statement : public Statement
{
	private:
		Expression *expression;

	public:
		Expression_Statement( Expression* _expression ) : expression(_expression) {}

		virtual void print_MIPS(std::ostream &dst, Context& context) const override
		{

			expression->print_MIPS(dst, context);

		}

};

class Compound_Statement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		Compound_Statement(std::vector<Declaration*>* _declaration_list = NULL, std::vector<Statement*>* _statement_list = NULL)
		:statement_list(_statement_list), declaration_list(_declaration_list) {}

		virtual void print_MIPS(std::ostream &dst, Context& context) const override
		{
			Context sub_context = Context(context);

			if(declaration_list != NULL)
			{
				for(auto it=declaration_list->begin(); it!=declaration_list->end(); it++)
				{
					(*it)->print_MIPS(dst, sub_context);
				}
			}

			if(statement_list != NULL)
			{
				for(auto it=statement_list->begin(); it!=statement_list->end(); it++){
					(*it)->print_MIPS(dst, sub_context);
				}
			}
		}
};

class Condition_If_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		Condition_If_Statement(Expression* _condition_expression, Statement* _true_statement)
		:condition_expression(_condition_expression),true_statement(_true_statement) {}

};

class Condition_If_Else_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* 	true_statement;
		Statement* 	false_statement;
		
	public:
		Condition_If_Else_Statement(Expression* _condition_expression, Statement* _true_statement, Statement* _false_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement), false_statement(_false_statement) {}

};

class While_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		While_Statement(Expression* _condition_expression, Statement* _true_statement)
		:condition_expression(_condition_expression),true_statement(_true_statement) {}

};

class For_Statement : public Statement
{
	private:
		Expression* initialisation_expression;
		Expression* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;
		
	public:
		For_Statement(Expression* _init_expr, Expression* _condition_expression, Expression* _update_expression, Statement* _true_statement)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

};

class Jump_Statement : public Statement
{
	private:
		Expression* expression;

	public:
		Jump_Statement( Expression* _expression = NULL)
		:expression(_expression) {}

};

#endif