#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include <string>
#include <iostream>
#include <map>

#include "ast_declaration.hpp"

/* ------------------------------------						   Expression Base Class					------------------------------------ */

class Expression : public Node 
{
	public:
		// Primitive functions
		virtual std::string get_variable_name() const { return 0; };
		virtual void load_variable_address(std::ostream &dst, Context& context) const {};
		virtual type get_data_type(Context& context) const { return type(INT); };
		virtual bool get_pointer_capability(Context& context) const { return 0; };
		virtual type_definition get_type_definition(Context& context) const {};

		// Evaluate the expression
		virtual double evaluate() const { return 0; };
};

typedef const Expression *Expression_Ptr;

/* ------------------------------------						     Unary Expression						------------------------------------ */

class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression(Expression* _expression) : expression(_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{ dst << "Triggered 1" << std::endl; }
};

/* ------------------------------------					    	 Sizeof Expressions						------------------------------------ */

class Sizeof_Type_Expression : public Expression
{
	private:
		type sizeof_type;
		
	public:
		Sizeof_Type_Expression(type_definition _sizeof_type) 
		: sizeof_type(_sizeof_type.get_variable_type()) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$2";

			// Select the size of the specified type
			if(sizeof_type == INT || sizeof_type == FLOAT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 4 << std::endl;
			}
			else if(sizeof_type == VOID)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(sizeof_type == CHAR)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(sizeof_type == DOUBLE)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(sizeof_type == FLOAT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(sizeof_type == UNSIGNED_INT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}

			context.store_register(dst, destination_register, stack_pointer);	
		}
};

class Sizeof_Variable_Expression : public Expression
{
	private:
		std::string variable_name;

	public:
		Sizeof_Variable_Expression(std::string _variable_name) 
		: variable_name(_variable_name) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Find the type of the variable
			type variable_type = context.get_variable(variable_name).get_variable_type();

			// Output size of specified type
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$2";

			// Select the size of the specified type
			if(variable_type == INT || variable_type == FLOAT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 4 << std::endl;
			}
			else if(variable_type == VOID)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 0 << std::endl;
			}
			else if(variable_type == CHAR)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(variable_type == DOUBLE)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(variable_type == FLOAT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(variable_type == UNSIGNED_INT)
			{
				dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}

			context.store_register(dst, destination_register, stack_pointer);	
		}
};

/* ------------------------------------						      Cast Expression						------------------------------------ */

class Cast_Expression : public Unary_Expression
{
	private:
		type casting_type;
	
	public:
		Cast_Expression(type_definition _casting_type, Expression* _expression) 
		: casting_type( _casting_type.get_variable_type()), Unary_Expression(_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			std::string cast_register = "$2";
			int cast_address = context.get_stack_pointer();

			expression->compile(dst, context);

			context.store_register(dst, cast_register, cast_address);
		}

		virtual type get_data_type(Context& context) const override { return casting_type; };
};

/* ------------------------------------						   	 Pre-fix Expression						------------------------------------ */

class Pre_Negative_Expression : public Unary_Expression
{
	private:

	public:
		Pre_Negative_Expression (Expression* _expression) 
		: Unary_Expression(_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Get stack pointer
			std::string destination_register = "$2";
			int stack_pointer = context.get_stack_pointer();

			expression->compile(dst, context);

			context.load_register(dst, destination_register, stack_pointer);
			//std::cerr<<expression->get_data_type(context)<<std::endl;
			if (expression->get_data_type(context) == INT )
			{
				dst << "\t" << "sub"  << "\t" << "\t" << destination_register << "," << "$0" << "," << destination_register << std::endl; 
			}
			else if (expression->get_data_type(context) == FLOAT)
			{
				context.shift_to_float_reg(dst, destination_register, "$f0");
				context.shift_to_float_reg(dst, "$0", "$f2");

				dst << "\t" << "sub.s"  << "\t" << "$f0" << "," << "$f2" << "," << "$f0" << std::endl; 

				context.shift_from_float_reg(dst, destination_register, "$f0");
			}

			context.store_register(dst, destination_register, stack_pointer);
		}
};

/* ------------------------------------						    Post-fix Expression						------------------------------------ */

class Post_Increment_Expression : public Unary_Expression
{
	private:
		Expression* increment_expression;
	public:
		Post_Increment_Expression (Expression* _expression, Expression* _increment_expression) 
		: Unary_Expression(_expression), increment_expression(_increment_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			expression->compile(dst, context);

			context.allocate_stack();
			increment_expression->compile(dst, context);
			context.deallocate_stack();
		}

		virtual type get_data_type(Context& context) const override { return increment_expression->get_data_type(context); };
};

class Post_Decrement_Expression : public Unary_Expression
{
	private:
		Expression* decrement_expression;
	public:
		Post_Decrement_Expression (Expression* _expression, Expression* _decrement_expression) 
		: Unary_Expression(_expression), decrement_expression(_decrement_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			expression->compile(dst, context);

			context.allocate_stack();
			decrement_expression->compile(dst, context);
			context.deallocate_stack();
		}

		virtual type get_data_type(Context& context) const override { decrement_expression->get_data_type(context); };
};

class Decrement_Expression : public Expression
{
	protected:
		Expression* left=NULL;
		Expression* right=NULL;

	public:
		Decrement_Expression (Expression* _left, Expression* _right) : left (_left), right (_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Only supports INT for now
			type operator_type = INT;

			// Select registers and allocate frame pointers
			int frame_pointer_1 = context.get_stack_pointer(); // Find current frame pointer 

			std::string temp_register = "$2";
			left->compile(dst, context);

			context.allocate_stack(); // Add new temprorary register
			int frame_pointer_2 = context.get_stack_pointer();
			std::string destination_register = "$8";
			right->compile(dst, context);

			context.deallocate_stack(); // Reduce frame pointer as temproray register done

			// Load registers 
			context.load_register(dst, temp_register, frame_pointer_1);
			context.load_register(dst, destination_register, frame_pointer_2);

			// Execute
			decrement(dst, context, operator_type, temp_register, destination_register);

			// Store result
			context.store_register(dst, temp_register, frame_pointer_1);
		}

		void decrement(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const
		{
			dst << "\t" << "subu" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}	

};

class Function_Call_Expression : public Unary_Expression
{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression(Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression(_expression), argument_list(_argument_list) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			
			// Get function parameters
			std::string variable_ID = expression->get_variable_name();
			int argument_size;

			int function_register_address = context.get_stack_pointer();
			std::string function_register = "$2";

			// Check for prototypes
			if (context.check_function_declared(variable_ID))
			{
				/* code */
			}
			else
			{
				// Declare the function 
				context.set_GLOBAL();
				context.new_variable(variable_ID, INT, FUNCTION);
				context.set_LOCAL();
			}
			
			if(argument_list != NULL)
			{
				argument_size = argument_list->size();

				// Handle four function argument
				for(int i = 0; i < argument_size; i++) { context.allocate_stack(); }
				
				int function_stack_pointer = context.get_stack_pointer();
				std::string argument_registers[4]  = {"$4", "$5", "$6", "$7"};

				// Store arguments
				int argument_stack_pointer = 0;
				for (int i = 0; i < argument_size; i++)
				{
					argument_stack_pointer += 4;

					// Temprorary registers
					context.allocate_stack();
					std::string temp_register = "$8";
					int temp_register_address = context.get_stack_pointer();

					(*argument_list)[i]->compile(dst, context);

					context.deallocate_stack();

					context.load_register(dst, temp_register, temp_register_address);
					dst << "\t" << "sw" << "\t" << "\t" << temp_register << "," << function_stack_pointer + argument_stack_pointer << "($30)" <<  std::endl; 
				}

				int argument_load_pointer = 0;
				int temp_register = 0;
				for(int i = 0; i < argument_size; i++)
				{
					argument_load_pointer += 4;

					if(i < 4)
					{
						dst << "\t" << "lw" << "\t" << "\t" << argument_registers[i] << "," << function_stack_pointer + argument_load_pointer << "($30)" <<  std::endl; 
					}
					else
					{
						temp_register = 4 + i;
						std::string temp_register_string = "$" + std::to_string(temp_register);
						dst << "\t" << "sw" << "\t" << "\t" << temp_register_string << "," << function_stack_pointer + argument_load_pointer - 4 << "($30)" <<  std::endl; 
					}
				}
			}
			

			// Go back to original function
			dst << "\t" << "addiu" << "\t" << "$29, $29," << context.get_stack_pointer() << std::endl; 
			dst << "\t" << "la"    << "\t" << "\t" << "$2," << variable_ID << std::endl;
			dst << "\t" << "jalr"  << "\t" << "$2" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$29, $29," << -context.get_stack_pointer() << std::endl; 

			for(int i = 0; i < argument_size; i++) { context.deallocate_stack(); }

			context.store_register(dst, function_register, function_register_address);
		}

		virtual type get_data_type(Context& context) const override
		{
			std::string variable_ID = expression->get_variable_name();

			if (context.check_function_declared(variable_ID)) { return (context.get_variable(variable_ID)).get_variable_type(); }
			else { return type(INT); }
		}
};

// https://stackoverflow.com/questions/54721000/c-array-indexing-in-mips-assembly

class Array_Access_Expression : public Unary_Expression
{
	private:
		Expression* array_expression;

	public:
		Array_Access_Expression(Expression* _expression, Expression* _array_expression)
		: Unary_Expression(_expression), array_expression(_array_expression) {}

		virtual void load_variable_address(std::ostream &dst, Context& context) const 
		{
			// Handle expression
			int array_frame_pointer = context.get_stack_pointer();
			std::string array_register = "$2";
			expression->compile(dst, context);

			// Handle array expression (Temporary register needed)
			context.allocate_stack();
			int temp_array_stack_pointer = context.get_stack_pointer();
			std::string temp_array_register = "$8";

			array_expression->compile(dst, context);

			context.deallocate_stack();

			context.load_register(dst, array_register, array_frame_pointer);
			context.load_register(dst, temp_array_register, temp_array_stack_pointer);

			dst << "\t" << "sll" << "\t" << "\t" << temp_array_register << "," << temp_array_register << "," << 2 << std::endl;
			dst << "\t" << "addu" << "\t" << array_register << "," << array_register << "," << temp_array_register << std::endl;

			context.store_register(dst, array_register, array_frame_pointer);
		};

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			type array_type = INT;
			// Add new temprorary register
			int array_frame_pointer = context.get_stack_pointer();
			std::string array_register = "$2";

			// Handle the expression
			load_variable_address(dst, context);

			dst << "\t" << "# Compile Array Access" << std::endl; 

			context.load_register(dst, array_register, array_frame_pointer);
			context.output_load_operation(dst, array_type, array_register, array_register, 0);

			// Array expression
			context.store_register(dst, array_register, array_frame_pointer);
		}

		virtual type get_data_type(Context& context) const override { return array_expression->get_data_type(context); }
};

/* ------------------------------------					    	Pointer Expressions						------------------------------------ */

class Reference_Expression : public Unary_Expression
{
	public:
		Reference_Expression(Expression* _expression) : Unary_Expression(_expression) {} 

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			expression->load_variable_address(dst, context);
		}

		virtual type get_data_type(Context& context) const override { return expression->get_data_type(context); }
};

// https://courses.cs.washington.edu/courses/cse378/09wi/lectures/lec03.pdf

class Dereference_Expression : public Unary_Expression
{
	public:
		Dereference_Expression(Expression* _expression) : Unary_Expression(_expression) {} 

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			std::string pointer_register = "$2";
			int pointer_address = context.get_stack_pointer();

			expression->compile(dst, context);

			context.load_register(dst, pointer_register, pointer_address);
			context.output_load_operation(dst, INT, pointer_register, pointer_register, 0);
			context.store_register(dst, pointer_register, pointer_address);
		}

		virtual type get_data_type(Context& context) const override { return expression->get_data_type(context); }
};

/* ------------------------------------						  Assignment Expressions					------------------------------------ */

class Assignment_Expression : public Expression // Base class
{
	protected:
		Expression* left_value;
		Expression* expression;
			
	public:
		Assignment_Expression (Expression* _left_value, Expression* _expression) : left_value (_left_value), expression (_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "Triggered 4" << std::endl;
		}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment (Expression* _left_value, Expression* _expression) : Assignment_Expression (_left_value, _expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			type operator_type = INT; // For now

			// Destination register set to $2 by default
			int frame_pointer_1 = context.get_stack_pointer();

			std::string destination_register = "$2";
			left_value->load_variable_address(dst, context);		

			dst << "\t" << "# Compile Direct Assignment Expression" << std::endl; 

			// Create temprorary register to handle expression
			context.allocate_stack();

			int frame_pointer_2 = context.get_stack_pointer(); 
			std::string temp_register_1 = "$8";
			expression->compile(dst, context);

			context.deallocate_stack();

			// Load registers 
			context.load_register(dst, destination_register, frame_pointer_1);
			context.load_register(dst, temp_register_1, frame_pointer_2);
			context.output_store_operation(dst, operator_type, temp_register_1, destination_register, 0);

			// Store results
			context.store_register(dst, temp_register_1, frame_pointer_1);
		}

		virtual type get_data_type(Context& context) const override { left_value->get_data_type(context); };
};

/* ------------------------------------						   Operator Expression						------------------------------------ */

// Arithmetic types
//	- Int  
// 	- Float
//	- Double 
//
// https://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html
// https://chortle.ccsu.edu/AssemblyTutorial/Chapter-31/ass31_2.html
// http://ww2.cs.fsu.edu/~dennis/teaching/2013_summer_cda3100/week5/week5-day2.pdf
// https://people.cs.pitt.edu/~childers/CS0447/lectures/SlidesLab92Up.pdf

class Operator : public Expression
{
	protected:
		Expression* left=NULL;
		Expression* right=NULL;

	public:
		Operator (Expression* _left, Expression* _right) : left (_left), right (_right) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			type operator_type = left->get_data_type(context);

			// Select registers and allocate frame pointers
			int frame_pointer_1 = context.get_stack_pointer(); // Find current frame pointer 

			std::string destination_register = "$2";
			left->compile(dst, context);

			context.allocate_stack(); // Add new temprorary register
			int frame_pointer_2 = context.get_stack_pointer();
			std::string temp_register = "$8";
			right->compile(dst, context);

			context.deallocate_stack(); // Reduce frame pointer as temproray register done

			// Load registers 
			context.load_register(dst, destination_register, frame_pointer_1);
			context.load_register(dst, temp_register, frame_pointer_2);

			// Execute
			execute(dst, context, operator_type, destination_register, temp_register);

			// Store result
			context.store_register(dst, destination_register, frame_pointer_1);

			if (operator_type == FLOAT || operator_type == DOUBLE)
			{
				context.load_float_register(dst, "$f0", frame_pointer_1);
			}
			
		}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const {}

		virtual type get_data_type(Context& context) const override { return type(left->get_data_type(context)); };

		virtual bool get_pointer_capability(Context& context) const { return left->get_pointer_capability(context); };
};

/* ------------------------------------						  Arithmetic Expression						------------------------------------ */

// https://people.cs.pitt.edu/~childers/CS0447/lectures/SlidesLab92Up.pdf

class Add_Expression : public Operator
{
	public:
		Add_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				dst << "\t" << "addu" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			}
			else if (type == FLOAT)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f0";
				std::string temp_float_reg_2 = "$f1";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "add.s" << "\t" << temp_float_reg_1 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_1);
			}
			else if (type == DOUBLE)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f2";
				std::string temp_float_reg_2 = "$f0";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "add.d" << "\t" << temp_float_reg_2 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_2);
			}
			else
			{
				std::cerr<<"Unsupported data types"<<std::endl;
				exit(1);
			}
		}

		virtual double evaluate() const override { return left->evaluate() + right->evaluate(); };
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				dst << "\t" << "subu" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			}
			else if (type == FLOAT)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f0";
				std::string temp_float_reg_2 = "$f1";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "sub.s" << "\t" << temp_float_reg_1 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_1);
			}
			else if (type == DOUBLE)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f2";
				std::string temp_float_reg_2 = "$f0";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "sub.d" << "\t" << temp_float_reg_2 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_2);
			}
			else
			{
				std::cerr<<"Unsupported data types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() - right->evaluate(); };
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				dst << "\t" << "multu" << "\t"  << destination_register << "," << temprorary_register << std::endl;
				dst << "\t" << "mflo"  << "\t"  << destination_register << std::endl;
			}
			else if (type == FLOAT)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f0";
				std::string temp_float_reg_2 = "$f1";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "mul.s" << "\t" << temp_float_reg_1 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_1);
			}
			else if (type == DOUBLE)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f2";
				std::string temp_float_reg_2 = "$f0";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "mul.d" << "\t" << temp_float_reg_2 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_2);
			}
			else
			{
				std::cerr<<"Unsupported data types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() * right->evaluate(); };
};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				dst << "\t" << "divu" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
				dst << "\t" << "mfhi" << "\t" << destination_register << std::endl;
				dst << "\t" << "mflo" << "\t" << destination_register << std::endl;
			}
			else if (type == FLOAT)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f0";
				std::string temp_float_reg_2 = "$f1";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "div.s" << "\t" << temp_float_reg_1 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_1);
			}
			else if (type == DOUBLE)
			{
				// Declare float registers
				std::string temp_float_reg_1 = "$f2";
				std::string temp_float_reg_2 = "$f0";

				// Shift to float registers
				context.shift_to_float_reg(dst, destination_register, temp_float_reg_1);
				context.shift_to_float_reg(dst, temprorary_register, temp_float_reg_2);

				// Add float
				dst << "\t" << "div.d" << "\t" << temp_float_reg_2 << "," << temp_float_reg_1 << "," << temp_float_reg_2 << std::endl;

				// Move result from float back to normal registers
				context.shift_from_float_reg(dst, destination_register, temp_float_reg_2);
			}
			else
			{
				std::cerr<<"Unsupported data types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() / right->evaluate(); };
};

/* ------------------------------------					  Relational Operator Expressions				------------------------------------ */

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
	
		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt " << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}	
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt " << "\t" << destination_register << "," << temprorary_register << "," << destination_register << std::endl;
		}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt "  << "\t" << destination_register << "," << temprorary_register << "," << destination_register << std::endl;
			dst << "\t" << "xori " << "\t" << "\t" << destination_register << "," << destination_register << "," << 0x1 << std::endl;
			dst << "\t" << "andi " << "\t" << "\t" << destination_register << "," << destination_register << "," << 0x00ff << std::endl;
		}	
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt "  << "\t" << destination_register << "," << temprorary_register << "," << destination_register << std::endl;
			dst << "\t" << "xori " << "\t" << "\t" << destination_register << "," << destination_register << "," << 0x1 << std::endl;
		}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{

			dst << "\t" << "xor " << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			dst << "\t" << "sltiu " << "\t" << destination_register << "," << destination_register << "," << 1 << std::endl;
		}	
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			
			dst << "\t" << "xor " << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			dst << "\t" << "sltu " << "\t" << destination_register << ",$0" << "," << destination_register << std::endl;
		}	
};

/* ------------------------------------					    Bitwise Operator Expressions				------------------------------------ */

class Bitwise_AND_Expression : public Operator
{
	public:
		Bitwise_AND_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "and " << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}	
};

class Bitwise_OR_Expression : public Operator
{
	public:
		Bitwise_OR_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "or " << "\t" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}	
};

class Bitwise_XOR_Expression : public Operator
{
	public:
		Bitwise_XOR_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "xor " << "\t" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}	
};

/* ------------------------------------					    Logical Operator Expressions				------------------------------------ */

class Logical_AND_Expression : public Operator
{
	public:
		Logical_AND_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Establish jump variables
			context.make_label("LOGICAL_AND");
			std::string return_label = context.get_function_return_label();

			std::string destination_name = "$2";
			int destination_address = context.get_stack_pointer();

			left->compile(dst, context);
			context.load_register(dst, destination_name, destination_address);

			dst << "\t" << "beq " << "\t" << destination_register << "," << "$0" << "," << return_label << std::endl;
			
			context.allocate_stack();
			int frame_pointer_temp_reg = context.get_stack_pointer();

			std::string temp_register_1 = "$8";
			right->compile(dst, context);

			context.load_register(dst, temp_register_1, frame_pointer_temp_reg);
			context.deallocate_stack();

			dst << "\t" << "beq " << "\t" << temp_register_1 << "," << "$0" << "," << return_label << std::endl;

			dst << "\t" << "addiu " << "\t" << "\t" << destination_register << "," << "$0" << "," << 1 << std::endl;

			context.store_register(dst, destination_name, destination_address);
		}

};

class Logical_OR_Expression : public Operator
{
	public:
		Logical_OR_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Establish jump variables
			std::string return_label_1 = context.make_label("LOGICAL_OR");
			std::string return_label_2 = context.make_label("LOGICAL_OR");

			std::string destination_name = "$2";
			int destination_address = context.get_stack_pointer();

			left->compile(dst, context);
			context.load_register(dst, destination_name, destination_address);

			dst << "\t" << "bne " << "\t" << "\t" << destination_register << "," << "$0" << "," << return_label_1 << std::endl;
			
			context.allocate_stack();
			int frame_pointer_temp_reg = context.get_stack_pointer();

			std::string temp_register_1 = "$8";
			right->compile(dst, context);

			context.load_register(dst, temp_register_1, frame_pointer_temp_reg);
			context.deallocate_stack();

			dst << "\t" << "beq " << "\t" << temp_register_1 << "," << "$0" << "," << return_label_2 << std::endl;

			dst << "\t" << "addiu " << "\t" << destination_register << "," << "$0" << "," << 1 << std::endl;


			dst << "\t" << return_label_1 << ":" << std::endl; 
			dst << "\t" << "addiu " << "\t" << destination_register << "," << "$0" << "," << 1 << std::endl;
			dst << "\t" << return_label_2 << ":" << std::endl; 

			context.store_register(dst, destination_name, destination_address);
		}
};

/* ------------------------------------					    	Bitwise Expressions						------------------------------------ */

class Left_Bitwise_Shift_Expression : public Operator
{
	public:
		Left_Bitwise_Shift_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			dst << "\t" << "sllv " << "\t" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
		}
};

class Right_Bitwise_Shift_Expression : public Operator
{
	public:
		Right_Bitwise_Shift_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			if (type == INT)
			{
				dst << "\t" << "srav " << "\t" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			}
			else
			{
				dst << "\t" << "sral " << "\t" << "\t" << destination_register << "," << destination_register << "," << temprorary_register << std::endl;
			}
		}
};

#endif