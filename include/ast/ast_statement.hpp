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
		}
};

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
			// Allocate temprorary registers
			context.allocate_stack();
			std::string temp_register_1 = "v0";
			int frame_pointer_1 = context.get_frame_pointer();

			// Execute the conditional statement
			condition_expression->compile(dst, context);

			// Deallocate
			context.deallocate_stack();

			// Handle conditional jumps
			std::string return_label = context.make_label("IF_RETURN");
			context.load_register(dst, temp_register_1, frame_pointer_1);

			dst << "\t" << "beq " << "\t" << "$0" << ",$" << temp_register_1 << "," << return_label << std::endl;

			true_statement->compile(dst, context);

			dst << return_label << ":" << std::endl;
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

};

class While_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		While_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement) {}

};

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
				int destination_address = context.get_frame_pointer();

				// Compile expression
				expression->compile(dst, context);

				// Deallocate
				context.deallocate_stack();

				// Move to return register
				std::string destination_register = "v0";
				context.load_register(dst, destination_register, destination_address);
				
				dst << "\t" << "move" << "\t" << "$v0" << ",$" << destination_register << std::endl;

				// Branch 
				dst << "\t" << "b " << context.get_function_return_label() << std::endl;
			}
		}

};

#endif