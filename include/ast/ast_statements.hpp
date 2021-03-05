#ifndef STATEMENTS
#define STATEMENTS
#include "ast_node.hpp"
#include "ast_expression.hpp"
#include "ast_declaration.hpp"

enum StatementEnum { CASE, DEFAULT, EXPRESSION, IF, ELSE, SWITCH, DO, WHILE, FOR, CONTINUE, BREAK, RETURN, COMPOUND };

/*struct Statement
{
  Statement(StatementEnum _type)                                          // Creates empty, continue, break, empty return and empty compound statements
    : type(_type)
  {}
  Statement(StatementEnum _type, Expression* _expression)                 // creates expression statements and return expression statements
    : type(_type)
  {
    expressions.push_back(_expression);
  }
  Statement(StatementEnum _type, Statement* _branch)                      // creates default statements
    : type(_type)
  {
    branches.push_back(_branch);
  }
  Statement(StatementEnum _type, Expression* _expression, Statement* _branch)                 // creates case, if, switch, while and do statements
    : type(_type)
  {
    expressions.push_back(_expression);
    branches.push_back(_branch);
  }
  Statement(StatementEnum _type, Expression* _expression, Statement* _branch1, Statement* _branch2)     // creates else statements
    : type(_type)
  {
    expressions.push_back(_expression);
    branches.push_back(_branch1);
    branches.push_back(_branch2);
  }
  Statement(StatementEnum _type, std::vector<Expression*> _expressions, Statement* _branch)             // creates for statements
    : type(_type)
    , expressions(_expressions)
  {
    branches.push_back(_branch);
  }
  Statement(StatementEnum _type, std::vector<Declaration*>* _declaration_list, std::vector<Statement*>* _statement_list)    // creates compound statements
    : type(_type)
    , declaration_list(_declaration_list)
    , statement_list(_statement_list)
  {}

  StatementEnum type;
  std::vector<Expression*> expressions;
  std::vector<Statement*> branches;
  std::vector<Declaration*>* declaration_list;
  std::vector<Statement*>* statement_list;
};*/

class Statement : public Node {};

class ExpressionStatement : public Statement
{
	private:
		Expression *expression;

	public:
		ExpressionStatement(Expression* _expression) : expression (_expression) {}

    ~ExpressionStatement()
    {
      delete expression;
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
		{
      int total_regs = context.size_in_words(expression->get_id());
      std::string destReg = context.alloc_reg(context.get_type(expression->get_id()), total_regs);
      expression->print_MIPS(dst, context, destReg);
      context.dealloc_reg(destReg, total_regs);
		}

};

class CompoundStatement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		CompoundStatement(std::vector<Declaration*>* _declaration_list=NULL, std::vector<Statement*>* _statement_list=NULL)
		: statement_list (_statement_list), declaration_list (_declaration_list) {}

		virtual void print_mips(std::ostream &dst, Context& context) const override
		{
			Context sub_context = Context(context);

			if (declaration_list)
			{
				for (auto declaration = declaration_list->begin(); declaration != declaration_list->end(); declaration++)
				{
					(*declaration)->print_mips(dst, sub_context);
				}
			}

			if (statement_list)
			{
				for(auto statement = statement_list->begin(); statement != statement_list->end(); statement++)
				{
					(*statement)->print_mips(dst, sub_context);
				}
			}
		}
};

class IfStatement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;

	public:
		Condition_If_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement) {}

};

class IfElseStatement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* 	true_statement;
		Statement* 	false_statement;

	public:
		Condition_If_Else_Statement (Expression* _condition_expression, Statement* _true_statement, Statement* _false_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement), false_statement (_false_statement) {}

};

class WhileStatement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;

	public:
		While_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement) {}

};

class ForStatement : public Statement
{
	private:
		Expression* initialisation_expression;
		Expression* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;

	public:
		For_Statement (Expression* _init_expr, Expression* _condition_expression, Expression* _update_expression, Statement* _true_statement)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

};

class JumpStatement : public Statement
{
	private:
		Expression* expression;

	public:
		Jump_Statement (Expression* _expression = NULL)
		: expression(_expression) {}

};

#endif
