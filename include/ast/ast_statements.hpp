#ifndef STATEMENTS
#define STATEMENTS
#include "ast_expressions.hpp"


class Statement : public Node                                  // complete
{
  protected:
    StatementEnum type;
};


class LabeledStatement : public Statement
{
  private:
    StatementEnum type;
    Expression* expression;
    Statement*	statement;

  public:
    LabeledStatement (StatementEnum _type, Statement* _statement, Expression* _expression=NULL)
    : type(_type), expression(_expression), statement(_statement) {}
};

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
      int total_regs = context.size_of(expression->get_id());
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
      int total_regs = context.size_of(condition->get_id());
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
      int total_regs = context.size_of(condition->get_id());
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
{
  private:
    StatementEnum type;
    Expression* expression;
    Statement*	statement;

  public:
    SwitchStatement (StatementEnum _type, Statement* _statement, Expression* _expression)
    : type(_type), expression(_expression), statement(_statement) {}
};

class WhileStatement : public Statement                        // should handle integral types
{
protected:
		Expression* condition;
		Statement*  statement;
    StatementEnum type;
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
      int total_regs = context.size_of(condition->get_id());
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
		Statement* initialisation_expression;
		Statement* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;

	public:
		ForStatement (Statement* _init_expr, Statement* _condition_expression, Statement* _true_statement, Expression* _update_expression=NULL)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

};

class JumpStatement : public Statement
{
	private:
		StatementEnum type;
	public:
		JumpStatement (StatementEnum _type)
		: type(_type) {}

};

class ReturnStatement : public Statement                       // complete
{
  private:
    Expression* expression;

  public:
    ReturnStatement (Expression* _expression = NULL)
    : expression(_expression) {}

    virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      if(expression){
        std::string destReg = "$2";
        expression->print_mips(dst, context, destReg);
      }
      int stack_size = context.get_stack_size_current();
      dst << "move $sp,$fp" << std::endl;
      dst << "lw $fp," << (stack_size-4) << "($sp)" << std::endl;
      dst << "addiu $sp,$sp," << (stack_size-8) << std::endl;
      dst << "j $31" << std::endl;
      dst << "nop" << std::endl;
    }
};

#endif
