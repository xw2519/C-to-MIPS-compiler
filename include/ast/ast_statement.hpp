#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP


#include <iomanip>

class Expression_Statement : public Statement
{
	private:
		Expression *expression;

	public:
		Expression_Statement ( Expression* _expression = NULL ) 
		: expression (_expression) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			if(expression != NULL)
			{	
				context.allocate_stack();
				expression->compile(dst, context);
				context.deallocate_stack();
			}
		}
};

class Compound_Statement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		Compound_Statement (std::vector<Declaration*>* _declaration_list = NULL, std::vector<Statement*>* _statement_list = NULL)
		: statement_list (_statement_list), declaration_list (_declaration_list) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			context.expand_context_scope();

			if (declaration_list != NULL)
			{
				for (auto declaration = declaration_list->begin(); declaration != declaration_list->end(); declaration++)
				{
					(*declaration)->compile(dst, context);
				}
			}

			if (statement_list != NULL)
			{
				for(auto statement = statement_list->begin(); statement != statement_list->end(); statement++)
				{
					(*statement)->compile(dst, context);
				}
			}

			context.shrink_context_scope();
		}
};

// https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture15/C_code.pdf

class Condition_If_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		Condition_If_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Allocate temporary registers
			context.allocate_stack();
			std::string temp_register_1 = "$2";
			int frame_pointer_1 = context.get_stack_pointer();

			// Execute the conditional statement
			condition_expression->compile(dst, context);

			// Deallocate
			context.deallocate_stack();

			// Handle conditional jumps
			std::string if_return_label = context.make_label("IF_RETURN");
			context.load_register(dst, temp_register_1, frame_pointer_1);

			dst << "\t" << "beq " << "\t" << "$0" << "," << temp_register_1 << "," << if_return_label << std::endl;

			true_statement->compile(dst, context);

			dst << if_return_label << ":" << std::endl;
		}
};

class Condition_If_Else_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* 	true_statement;
		Statement* 	false_statement;
		
	public:
		Condition_If_Else_Statement (Expression* _condition_expression, Statement* _true_statement, Statement* _false_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement), false_statement (_false_statement) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Allocate temporary registers
			context.allocate_stack();
			std::string temp_register_1 = "$2";
			int frame_pointer_1 = context.get_stack_pointer();

			// Execute the conditional statement
			condition_expression->compile(dst, context);
			context.load_register(dst, temp_register_1, frame_pointer_1);

			// Deallocate
			context.deallocate_stack();

			// Handle conditional jumps
			std::string if_return_label = context.make_label("IF_RETURN");
			std::string else_return_label = context.make_label("ELSE_RETURN");

			dst << "\t" << "beq " << "\t" << "$0" << "," << temp_register_1 << "," << if_return_label << std::endl;

			true_statement->compile(dst, context);

			dst << "\t" << "b " << "\t" << "\t" << else_return_label << std::endl;

			dst << std::endl;
			dst << if_return_label << ":" << std::endl;

			false_statement->compile(dst, context);

			dst << std::endl;
			dst << else_return_label << ":" << std::endl;

		}

};

class While_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		While_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Set loop boundaries
			std::string START_label = context.make_label("START_WHILE");
			std::string FINISH_label = context.make_label("FINISH_WHILE");

			dst << START_label << ":" << std::endl;

			// Allocate temporary registers
			context.allocate_stack();
			std::string temp_register_1 = "$2";
			int frame_pointer_1 = context.get_stack_pointer();
			context.load_register(dst, temp_register_1, frame_pointer_1);

			// Execute the conditional statement
			condition_expression->compile(dst, context);

			// Deallocate
			context.deallocate_stack();
			
			dst << "\t" << "beq " << "\t" << "$0" << "," << temp_register_1 << "," << FINISH_label << std::endl;

			true_statement->compile(dst, context);

			dst << "\t" << "b " << "\t"  << "\t" << START_label << std::endl;
			dst << "\t" << FINISH_label << ":" << std::endl;
		}

};

// https://courses.engr.illinois.edu/cs232/fa2011/section/disc1.pdf

class For_Statement : public Statement
{
	private:
		Expression* initialisation_expression;
		Expression* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;
		
	public:
		For_Statement (Expression* _init_expr, Expression* _condition_expression, Expression* _update_expression, Statement* _true_statement)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Set loop boundaries
			std::string START_label = context.make_label("START_FOR");
			std::string FINISH_label = context.make_label("FINISH_FOR");

			// Handle initialisation expressions
			if(initialisation_expression != NULL)
			{
				context.allocate_stack();
				int stack_pointer_1 = context.get_stack_pointer();

				initialisation_expression->compile(dst, context);

				context.deallocate_stack();
			}

			dst << std::endl;
			dst << START_label << ":" << std::endl;
			dst << std::endl;

			// Allocate temporary registers
			context.allocate_stack();
			std::string temp_condition_reg = "$8";
			int stack_pointer_2 = context.get_stack_pointer();

			if(condition_expression != NULL)
			{
				condition_expression->compile(dst, context);
			}

			context.load_register(dst, temp_condition_reg, stack_pointer_2);

			// Deallocate
			context.deallocate_stack();

			dst << "\t" << "beq " << "\t" << "$0" << "," << temp_condition_reg << "," << FINISH_label << std::endl;

			true_statement->compile(dst, context);

			if(update_expression != NULL)
			{
				context.allocate_stack();
				std::string temp_update_reg = "$8";

				update_expression->compile(dst, context);
				
				context.deallocate_stack();
			}

			dst << "\t" << "b " << "\t" << "\t" << START_label << std::endl;

			dst << std::endl;
			dst << FINISH_label << ":" << std::endl;
			dst << std::endl;
		}

};

class Jump_Statement : public Statement
{
	private:
		Expression* expression;

	public:
		Jump_Statement (Expression* _expression = NULL)
		: expression(_expression) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Check if there is an expression attached
			if(expression != NULL)
			{
				// Allocate 
				context.allocate_stack();
				int destination_address = context.get_stack_pointer();

				// Compile expression
				expression->compile(dst, context);

				// Deallocate
				context.deallocate_stack();

				// Move to return register
				std::string destination_register = "$2";
				context.load_register(dst, destination_register, destination_address);
				
				// dst << "\t" << "move" << "\t" << "$$2" << ",$" << destination_register << std::endl;

				// Branch 
				dst << "\t" << "b " << "\t"  << "\t" << context.get_function_return_label() << std::endl;
			}
		}

};

// http://web.engr.oregonstate.edu/~walkiner/cs271-wi13/slides/07-MoreAssemblyProgramming.pdf

class Break_Statement : public Statement
{
	private:

	public:
		Break_Statement() {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Branch 
			dst << "\t" << "b " << "\t"  << "\t" << context.get_break_label() << std::endl;
		}
};

class Continue_Statement : public Statement
{
	private:

	public:
		Continue_Statement() {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			// Branch 
			dst << "\t" << "b " << "\t"  << "\t" << context.get_continue_label() << std::endl;
		}
};

#endif