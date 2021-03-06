#ifndef STATEMENTS
#define STATEMENTS
#include "ast_node.hpp"
#include "ast_expressions.hpp"
#include "ast_declarations.hpp"

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

class Statement : public Node
{
  protected:
    StatementEnum type;
};

class CompoundStatement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		CompoundStatement(std::vector<Declaration*>* _declaration_list=NULL, std::vector<Statement*>* _statement_list=NULL)
		: statement_list (_statement_list), declaration_list (_declaration_list) { type = COMPOUND; }

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

class LabeledStatement : public Statement
{};

class ExpressionStatement : public Statement                   // complete
{
	private:
		Expression *expression;

	public:
		ExpressionStatement(Expression* _expression=NULL)
    : expression (_expression) { type = EXPR_STMT; }

    ~ExpressionStatement()
    {
      delete expression;
    }

    virtual bool is_null() const
    {
      return (expression==NULL);
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
		{
      int total_regs = context.size_in_words(expression->get_id());
      std::string destReg = context.alloc_reg(context.get_type(expression->get_id()), total_regs);
      expression->print_mips(dst, context, destReg);
      context.dealloc_reg(destReg, total_regs);
		}

    virtual void print_mips(std::ostream &dst, Context& context, std::string destReg) const
    {
      expression->print_mips(dst, context, destReg);
    }

};

class IfStatement : public Statement                           // should handle integral types
{
	private:
		Expression* condition;
		Statement* statement;

	public:
		IfStatement (Expression* _condition, Statement* _statement)
		: condition (_condition), statement (_statement) { type = IF; }

    ~IfStatement()
    {
      delete condition;
      delete statement;
    }

    virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      int total_regs = context.size_in_words(condition->get_id());
      std::string destReg = context.alloc_reg(INT, total_regs);
      std::string label = context.make_label();

      condition->print_mips(dst, context, destReg);
      dst << "beq " << destReg << ",$0," << label << std::endl;
      dst << "nop" << std::endl;
      statement->print_mips(dst, context);
      dst << label << ":" << std::endl;

      context.dealloc_reg(destReg, total_regs);
    }
};

class ElseStatement : public Statement                         // should handle integral types
{
	private:
		Expression* condition;
		Statement* 	true_statement;
		Statement* 	false_statement;

	public:
		ElseStatement (Expression* _condition, Statement* _true_statement, Statement* _false_statement)
		: condition (_condition), true_statement (_true_statement), false_statement (_false_statement) { type = ELSE; }

    ~ElseStatement()
    {
      delete condition;
      delete true_statement;
      delete false_statement;
    }

    virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      int total_regs = context.size_in_words(condition->get_id());
      std::string destReg = context.alloc_reg(INT, total_regs);
      std::string true_label = context.make_label();
      std::string false_label = context.make_label();

      condition->print_mips(dst, context, destReg);
      dst << "beq " << destReg << ",$0," << false_label << std::endl;
      dst << "nop" << std::endl;
      true_statement->print_mips(dst, context);
      dst << "b " << true_label << std::endl;
      dst << "nop" << std::endl;
      dst << false_label << ":" << std::endl;
      false_statement->print_mips(dst, context);
      dst << true_label << ":" << std::endl;

      context.dealloc_reg(destReg, total_regs);
    }

};

class SwitchStatement : public Statement
{};

class WhileStatement : public Statement                        // should handle integral types
{
	private:
		Expression* condition;
		Statement*  statement;

	public:
		WhileStatement (StatementEnum _type, Expression* _condition, Statement* _statement)
		: type(_type), condition(_condition), statement(_statement) {}

    ~WhileStatement()
    {
      delete condition;
      delete statement;
    }

    virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      int total_regs = context.size_in_words(condition->get_id());
      std::string destReg = context.alloc_reg(INT, total_regs);
      std::string begin_label = context.make_label();
      if(type==WHILE){
        std::string exit_label = context.make_label();

        dst << begin_label << ":" << std::endl;
        condition->print_mips(dst, context, destReg);
        dst << "beq " << destReg << ",$0," << exit_label << std::endl;
        dst << "nop" << std::endl;
        statement->print_mips(dst, context);
        dst << "b " << begin_label << std::endl;
        dst << "nop" << std::endl;
        dst << exit_label << ":" << std::endl;
      }else{
        dst << begin_label << ":" << std::endl;
        statement->print_mips(dst, context);
        condition->print_mips(dst, context, destReg);
        dst << "bne " << destReg << ",$0," << begin_label << std::endl;
        dst << "nop" << std::endl;
      }
      context.dealloc_reg(destReg, total_regs);
    }

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

class ReturnStatement : public Statement
{};

#endif
