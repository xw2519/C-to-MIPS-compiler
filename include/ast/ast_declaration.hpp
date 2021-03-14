#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP


#include <iomanip>

#include "ast_expression.hpp"

// https://stackoverflow.com/questions/7748726/declarators-in-c

class Statement : public Node {};

/* ------------------------------------						     Program Class						------------------------------------ */

class Program : public Node
{
	private:
		Node_Ptr left;
		Node_Ptr right;

	public:
		Program(Node_Ptr _left, Node_Ptr _right) : left(_left), right(_right) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			left ->compile(dst, context);
			right->compile(dst, context);
		}
};

/* ------------------------------------						       Base Class						------------------------------------ */

class External_Declaration : public Node {};

/* ------------------------------------						    Declarator Class					------------------------------------ */

class Declarator : public External_Declaration 
{
	public:
		virtual std::string get_variable_name() {}
		virtual int get_variable_size() {}

		virtual void compile_declaration(std::ostream &dst, Context& context, type declarator_type) const {}
		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const {}
		virtual void compile_declaration_array_initialisation(std::ostream &dst, Context& context, type declarator_type, std::vector<Expression*>* expression_vector) const {}
};

class Variable_Declarator : public Declarator
{
	private:
		std::string variable_name;

	public:
		Variable_Declarator(std::string _variable_name) : variable_name(_variable_name) {}

		// Get functions
		virtual std::string get_variable_name() { return variable_name; }
		virtual int get_variable_size() { return 8; }

		// Print MIPS
		virtual void compile_declaration(std::ostream& dst, Context& context, type declaration_type) const override 
		{
			// Get necessary information
			variable compile_variable = context.new_variable(variable_name, INT, NORMAL);

			type variable_type = compile_variable.get_variable_type();
			int frame_pointer_1 = compile_variable.get_variable_address();

			// Print MIPS
			context.output_store_operation(dst, variable_type, "$0", "$30", frame_pointer_1);
		}

		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const override 
		{
			variable declared_variable = context.new_variable(variable_name, declarator_type, NORMAL);

			if(context.get_context_scope() == LOCAL)
			{
				// Allocate temporary registers
				context.allocate_stack();
				int frame_pointer_1 = context.get_stack_pointer();
				std::string temp_register_1 = "$8";
				
				expressions->compile(dst, context);

				// Deallocate
				context.deallocate_stack();

				context.load_register(dst, temp_register_1, frame_pointer_1);
				context.output_store_operation(dst, declarator_type, temp_register_1, "$30", declared_variable.get_variable_address());
			}
		}
};

// https://stackoverflow.com/questions/54721000/c-array-indexing-in-mips-assembly
// https://cgi.cse.unsw.edu.au/~cs1521/20T2/lec/mips_data/notes

class Array_Declarator : public Declarator 
{
	private: 
		std::string variable_name;
		int array_size;

	public:
		Array_Declarator(std::string _variable_name, Expression *_array_size_expression) 
		{
			variable_name = _variable_name;

			if(_array_size_expression != NULL) { array_size = _array_size_expression->evaluate(); }
			else { array_size = -1; }
		}

		// Get functions
		virtual std::string get_variable_name() { return variable_name; }
		virtual int get_variable_size() { return array_size; }

		// Print MIPS
		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int array_frame_pointer = 0;

			if(context.get_context_scope() == LOCAL)
			{
				dst << "\t" << "# Store array content locally" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{				
					array_frame_pointer =  array_size + (i*8);

					// Print MIPS
					context.output_store_operation(dst, INT, "$0", "$30", array_frame_pointer);
				}
			}
			else
			{
				dst << "\t" << "# Store array content globally" << std::endl; 

				dst << "\t" << ".globl" << "\t" << variable_name << std::endl;
				dst << "\t" << ".data"  << std::endl;

				dst <<  variable_name 	<< ":" << std::endl;

				for(int i = 0; i < array_size; i++)
				{				
					dst << "\t" << ".space " << array_size*8 << std::endl;
				}
			}

			
		}

		virtual void compile_declaration(std::ostream& dst, Context& context, type declaration_type) const override 
		{
			// Get necessary information
			variable array_variable = context.new_variable(variable_name, INT, ARRAY, array_size);
			
			int array_frame_pointer = 0;

			if(context.get_context_scope() == LOCAL)
			{
				dst << "\t" << "# Store array content" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{
					array_frame_pointer =  array_variable.get_variable_address() + (i*8);

					// Print MIPS	
					context.output_store_operation(dst, INT, "$0", "$30", array_frame_pointer);
				}
			}
			else
			{
				dst << "\t" << "# Store array content globally" << std::endl; 
		
				dst << "\t" << ".comm " << variable_name << " " << array_size*8 << std::endl;
			}
		}	

		virtual void compile_declaration_array_initialisation(std::ostream &dst, Context& context, type declarator_type, std::vector<Expression*>* expression_vector) const 
		{
			// Get array parameters
			int initialisation_vector_size = expression_vector->size();

			variable array_variable = context.new_variable(variable_name, INT, ARRAY, initialisation_vector_size);

			for(int i = 0; i < array_size; i++)
			{
				// Handle initialisations 
				if(i < initialisation_vector_size)
				{
					// Find offset relative to the array
					int array_offset = array_variable.get_variable_address() + (i*8);

					// Allocate storage
					context.allocate_stack();
					int stack_pointer = context.get_stack_pointer();
					std::string init_register = "$8";

					(*expression_vector)[i]->compile(dst, context);

					context.deallocate_stack();

					// Output assembly instructions for loading and storage
					context.load_register(dst, init_register, stack_pointer);
					context.output_store_operation(dst, INT, init_register, "$30", array_offset);
				}
				else
				{
					// Find offset relative to the array
					int array_offset = array_variable.get_variable_address() + (i*8);

					context.output_store_operation(dst, INT, "$0", "$30", array_offset);
				}
			}
		}
};

/* ------------------------------------					 Initialisation Declarator Class			------------------------------------ */

class Initialisation_Variable_Declarator : public Declarator 
{
	// Expressions like "int a = 4 + 7 + b"
	private: 
		Declarator* initialisation_declarator;
		Expression* initialisation_expressions;

	public: 
		Initialisation_Variable_Declarator(Declarator* _initialisation_declarator, Expression* _initialisation_expressions)
		: initialisation_declarator(_initialisation_declarator), initialisation_expressions(_initialisation_expressions) {}

		virtual void compile_declaration(std::ostream &dst, Context& context, type declaration_type) const override
		{
			initialisation_declarator->compile_declaration_initialisation(dst, context, declaration_type, initialisation_expressions);
		}
};

class Initialisation_Array_Declarator : public Declarator 
{
	// Expressions like "int a = 4 + 7 + b"
	private: 
		Declarator* initialisation_declarator;
		std::vector<Expression*>* initialisation_vector;

	public: 
		Initialisation_Array_Declarator(Declarator* _initialisation_declarator, std::vector<Expression*>* _initialisation_vector)
		: initialisation_declarator(_initialisation_declarator), initialisation_vector(_initialisation_vector) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			std::string initialisation_variable_name = initialisation_declarator->get_variable_name();
			int initialisation_array_size = initialisation_declarator->get_variable_size();

			variable array_variable = context.get_variable(initialisation_variable_name);

			for(int i = 0; i < initialisation_array_size; i++)
			{				
				// Find offset relative to the array
				int array_offset = array_variable.get_variable_address() + (i*8);

				// Allocate storage
				context.allocate_stack();
				int stack_pointer = context.get_stack_pointer();
				std::string init_register = "$8";

				(*initialisation_vector)[i]->compile(dst, context);

				context.deallocate_stack();

				// Output assembly instructions for loading and storage
				context.load_register(dst, init_register, stack_pointer);
				context.output_store_operation(dst, INT, init_register, "$30", array_offset);
			}
		}

		virtual void compile_declaration(std::ostream &dst, Context& context, type declaration_type) const override
		{
			initialisation_declarator->compile_declaration_array_initialisation(dst, context, declaration_type, initialisation_vector);
		}
};

/* ------------------------------------						    Declaration Class					------------------------------------ */

class Declaration : public External_Declaration
{
	private:
		type TYPE;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(type_definition _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: TYPE(_TYPE.get_variable_type()), declaration_list(_declaration_list) {}

		virtual std::string get_parameter()
		{
			return (*declaration_list)[0]->get_variable_name();
		}

		type get_type() { return TYPE; }

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			if (declaration_list != NULL)
			{				
				for (int i = 0; i < declaration_list->size(); i++)
				{
					Declarator* temp_declarator = declaration_list->at(i);
					
					(*temp_declarator).compile_declaration(dst, context, TYPE);
					
				}
			}
		}

};

/*

Handles the allocation and deallocation of stack and all necessary assembly formatting (referenced mostly from GodBolt). Each 
called function in a program allocates a stack frame on the run-time stack, ifnecessary.  

The formatting is: 
	- Opening directives
	- Function 
		- Stack allocation
		- Assembly code
		- Stack deallocation
	- Closing directives

Note that stack grows downward i.e. push items onto the stack, the address decreases. The relative value of the stack 
pointer at the beginning of execution is taken as zero.
- The stack is set to -8 but default and defined as (variable_counter*4) if variable_counter is more than 2

*/

// https://courses.cs.washington.edu/courses/cse378/02sp/sections/section3-1.html

class Function_Definition : public External_Declaration // Very basic
{
	private:
		type TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement* statements;

	public:
		Function_Definition (type_definition _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement *_statements) 
		: TYPE(_TYPE.get_variable_type()), ID(_ID), parameter_list(_parameter_list), statements(_statements) {}

		virtual void compile(std::ostream& dst, Context& context) const override
		{
			/* -------------------------------- 		  Function preparation 			-------------------------------- */

			context.expand_context_scope();

			context.set_LOCAL();

			// Handle function return 
			context.set_main_return_label("RETURN");

			/* -------------------------------- 		   Opening directives 			-------------------------------- */
			dst << std::endl;
			dst << "# ------------ Opening directives ------------ #" << std::endl;
			dst << std::endl;

			dst << "\t" << ".text"  << std::endl;
			dst << "\t" << ".globl" << "\t" << ID << std::endl;
			dst << "\t" << ".ent"   << "\t" << ID << std::endl;
			dst << "\t" << ".type"  << "\t" << ID <<", @function" << std::endl;

			dst << std::endl;
			dst << "# ------------ Function call ------------ #" << std::endl;
			dst << std::endl;

			dst <<  ID  << ":" << std::endl;

			dst << std::endl;
			dst << "# ------------ Allocate stack frame ------------ #" << std::endl; // Allocate basic stack size before adjustment
			dst << std::endl;

			dst << "\t" << "sw"    << "\t" << "\t" << "$31,"	   << "-8"  << "($29)" << std::endl;
			dst << "\t" << "sw"    << "\t" << "\t" << "$30,"	   << "-16" << "($29)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$29,$29,"   		   << "-16" << std::endl; 
			dst << "\t" << "move"  << "\t" << "$30,$29"    		   << std::endl;

			dst << std::endl;
			dst << "# ------------ Program Assembly ------------ #" << std::endl;
			dst << std::endl;

			// Function arguments
			if(parameter_list != NULL)
			{
				int argument_stack_pointer = 8; // Set to 8 to prevent stack frame clash

				std::string argument_registers[4]  = {"$4", "$5", "$6", "$7"};

				// Check if parameters can fit into four argument register
				// https://stackoverflow.com/questions/2298838/mips-function-call-with-more-than-four-arguments

				int temp_register = 4;
				for(int i = 0; i < parameter_list->size(); i++)
				{
					argument_stack_pointer += 8;

					// Check if argument stack is full or not based on argument_stack_pointer
					if(i < 4) 
					{
						context.output_store_operation(dst, INT, argument_registers[i], "$30", argument_stack_pointer);
					}
					else // Store parameters on stack 
					{
						
						temp_register = 4 + i;
						std::string temp_register_string = "$" + std::to_string(temp_register);
						context.output_store_operation(dst, INT, temp_register_string, "$30", argument_stack_pointer);
					}

					context.make_new_argument((*parameter_list)[i]->get_parameter(), INT, NORMAL, argument_stack_pointer);
				}

			}
			
			// Function body
			if(statements != NULL)
			{ 
				statements->compile(dst, context);
			}
			
			
			if (ID == "main")
			{
            	dst << "\t" << "move" << "\t" << "$2, $0" << std::endl; 
        	}
			
			dst << std::endl;
			dst << context.get_function_return_label() << ":" << std::endl; 

			dst << std::endl;		
			dst << "# ------------ Deallocate stack frame ------------ #" << std::endl;
			dst << std::endl;

			dst << "\t" << "move"  << "\t" << "$29, $30"  << std::endl; 
			dst << "\t" << "addiu" << "\t" << "$29, $29," << "16" << std::endl; 
			dst << "\t" << "lw"    << "\t" << "\t" << "$30," << "-16" << "($29)" << std::endl;
			dst << "\t" << "lw"    << "\t" << "\t" << "$31," << "-8" << "($29)" << std::endl;
			dst << "\t" << "j" 	   << "\t" << "\t" << "$31"  << std::endl;

			dst << std::endl;
			dst << "# ------------ Closing directives ------------ #" << std::endl;
			dst << std::endl;
			dst << "\t" << ".end" << "\t" << ID << std::endl;

			context.deallocate_stack();
			context.shrink_context_scope();
			context.set_GLOBAL();
		}
};

class Function_Prototype_Declaration : public Declarator
{
	private:
		std::string function_name;
		std::vector<Declaration*>* parameter_list;
	
	public:
		Function_Prototype_Declaration(std::string _function_name, std::vector<Declaration*>* _parameter_list)
		: function_name(_function_name), parameter_list(_parameter_list) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			context.new_variable(function_name, INT, FUNCTION);
		}
};


#endif