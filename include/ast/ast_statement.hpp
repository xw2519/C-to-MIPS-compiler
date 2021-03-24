#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP


#include <iomanip>

/* ------------------------------------						   	 Base Class					------------------------------------ */

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

/* ------------------------------------						   Conditional Class			------------------------------------ */

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
			
			// Execute the conditional statement
			condition_expression->compile(dst, context);

			// Deallocate
			context.deallocate_stack();

			// Handle conditional jumps
			std::string if_return_label = context.make_label("IF_RETURN");

			// Compare and branch
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

/* ------------------------------------						   	  Loop Class				------------------------------------ */

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
				context.allocate_stack();

				int destination_address = context.get_stack_pointer();
				// Compile expression
				expression->compile(dst, context);

				context.deallocate_stack();

				destination_address = context.get_stack_pointer();

				// Return classification 
				// context.get_variable().get_pointer_capability()

				if(context.get_variable_address(destination_address).get_pointer_capability())
				{
					//context.load_register(dst, "$2", destination_address);
				}
				
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
			dst << "\t" << "b " << "\t"  << "\t" << context.get_break_label().top() << std::endl;
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
			dst << "\t" << "b " << "\t"  << "\t" << context.get_continue_label().top() << std::endl;
		}
};

/* ------------------------------------						     Switch Class					------------------------------------ */

class Switch_Statement : public Statement
{
	private:
		Expression* expression;
		Statement* switch_statements;

	public:
		Switch_Statement (Expression* _expression = NULL, Statement* _switch_statements = NULL)
		: expression(_expression), switch_statements(_switch_statements) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{			
			dst << std::endl;
			dst << "# ------------ Switch statement ------------ #" << std::endl;

			// Set loop boundaries
			std::string START_label = context.make_label("START_SWITCH");
			std::string FINISH_label = context.make_label("FINISH_SWITCH");

			context.add_break_label(FINISH_label);

			// Label declarations 
			std::stringstream ss; 
			switch_statements->compile(ss, context);

			// Handle switch expression
			context.allocate_stack();
			std::string switch_register = "$2";
			int switch_stack_pointer = context.get_stack_pointer();
			expression->compile(dst, context);

			// Handle the case statements
			context.allocate_stack();
			std::string case_register = "$8";
			int case_stack_pointer = context.get_stack_pointer();
			
			while(context.get_case_statement_size() != 0)
			{
				Expression* case_statement = context.get_case_statement();
				case_statement->compile(dst, context);
				std::string case_label = context.get_case_label();

				context.load_register(dst, switch_register, switch_stack_pointer);
				context.load_register(dst, case_register, case_stack_pointer);

				dst << "\t" << "beq" << "\t" << switch_register << "," << case_register << "," << case_label << std::endl;

				context.remove_case_statement();
				context.remove_label_statement();
			}

			// Handle default statement
			if(context.get_case_label_size() != 0)
			{
				dst << "\t" << "b " << "\t"  << "\t" << context.get_case_label() << std::endl;
				context.remove_label_statement();
			}

			dst << "\t" << "b " << "\t"  << "\t" << FINISH_label << std::endl;

			// Print out label declarations
			dst << ss.str() << std::endl;

			dst << FINISH_label << ":" << std::endl;

			// Clean up
			context.deallocate_stack();
			context.deallocate_stack();
			context.remove_break_label();
		}
};

class Case_Statement : public Statement
{
	private:
		Expression* expression;
		Statement* switch_statements;

	public:
		Case_Statement( Expression* _expression, Statement* _switch_statements)
		: expression(_expression), switch_statements(_switch_statements) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "# ------------ Case statement ------------ #" << std::endl;

			// Generate case label
			std::string case_label = context.make_label("CASE");

			// Store case label and statements
			context.add_case_statements(expression, case_label);

			// Label declaration
			dst << case_label << ":" << std::endl;

			// Compile statements
			switch_statements->compile(dst, context);
		}
};

class Default_Statement : public Statement
{
	private:
		Statement* default_statements;

	public:
		Default_Statement(Statement* _default_statements)
		: default_statements(_default_statements) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "# ------------ Default statement ------------ #" << std::endl;
			// Generate case label
			std::string default_label = context.make_label("DEFAULT");

			// Store case label for reference
			context.add_case_label(default_label);

			// Label declaration
			dst << default_label << ":" << std::endl;

			// Compile statements
			default_statements->compile(dst, context);
		}
};

#endif