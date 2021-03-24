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
		virtual std::string get_variable_name() { return ""; }
		virtual int get_variable_size() { return 0; }

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const {}
		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type_definition declarator_type, Expression* expressions, bool pointer_capable) const {}
		virtual void compile_declaration_array_initialisation(std::ostream &dst, Context& context, type_definition declarator_type, std::vector<Expression*>* expression_vector) const {}
};

class Variable_Declarator : public Declarator
{
	private:
		std::string variable_name;
		bool pointer_capable;

	public:
		Variable_Declarator(std::string _variable_name, bool _pointer_capable) 
		: variable_name(_variable_name), pointer_capable(_pointer_capable) {}

		Variable_Declarator(std::string _variable_name) 
		: variable_name(_variable_name) {}

		// Get functions
		virtual std::string get_variable_name() { return variable_name; }
		virtual int get_variable_size() { return 4; }

		// Print MIPS
		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override 
		{
			type variable_type;
			int frame_pointer_1;

			if (pointer_capable)
			{
				variable compile_variable = context.new_variable(variable_name, declarator_type, NORMAL, pointer_capable);
				variable_type = compile_variable.get_variable_type();
				frame_pointer_1 = compile_variable.get_variable_address();
			}
			else
			{
				// Get necessary information
				variable compile_variable = context.new_variable(variable_name, declarator_type, NORMAL);
				variable_type = compile_variable.get_variable_type();
				frame_pointer_1 = compile_variable.get_variable_address();
			}

			// Print MIPS
			context.output_store_operation(dst, variable_type, "$0", "$30", frame_pointer_1);
		}

		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type_definition declarator_type, Expression* expressions, bool pointer_capable) const override 
		{
			variable declared_variable = context.new_variable(variable_name, declarator_type, NORMAL, pointer_capable);

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
				context.output_store_operation(dst, declarator_type.get_variable_type(), temp_register_1, "$30", declared_variable.get_variable_address());
			}
			else if(context.get_context_scope() == GLOBAL)
			{
				int expression = 0;

				if (expressions != NULL)
				{
					expression = expressions->evaluate();
				}
			
				dst << "# ------------ Global declaration ------------ #" << std::endl;
				dst << "\t" << ".globl" << "\t" << variable_name << std::endl;
				dst << "\t" << ".data" << std::endl;
				dst <<  variable_name  << ":" << std::endl;
				dst << "\t" << "." << declared_variable.get_storage_type() << " " << expression << std::endl;
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

			variable array_variable = context.get_variable(variable_name);

			if(context.get_context_scope() == LOCAL)
			{
				dst << "\t" << "# Store array content locally" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{				
					array_frame_pointer =  array_size + (i*4);

					// Print MIPS
					context.output_store_operation(dst, array_variable.get_variable_type(), "$0", "$30", array_frame_pointer);
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
					dst << "\t" << ".space " << array_size*4 << std::endl;
				}
			}

			
		}

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override 
		{
			declarator_type.increment_array_tracker();

			// Get necessary information
			variable array_variable = context.new_variable(variable_name, declarator_type, ARRAY, 0, array_size);
			
			int array_frame_pointer = 0;

			if(context.get_context_scope() == LOCAL)
			{
				dst << "\t" << "# Store array content" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{
					array_frame_pointer = array_variable.get_variable_address() + (i*4);
					context.output_store_operation(dst, declarator_type.get_variable_type(), "$0", "$30", array_frame_pointer);
				}
			}
			else
			{
				dst << "\t" << "# Store array content globally" << std::endl; 
				dst << "\t" << ".comm " << variable_name << " " << array_size*4 << std::endl;
			}
		}	

		virtual void compile_declaration_array_initialisation(std::ostream &dst, Context& context, type_definition declarator_type, std::vector<Expression*>* expression_vector) const 
		{
			// Get array parameters
			int initialisation_vector_size = expression_vector->size();

			variable array_variable = context.new_variable(variable_name, declarator_type, ARRAY, initialisation_vector_size);

			for(int i = 0; i < array_size; i++)
			{
				// Handle initialisations 
				if (i < initialisation_vector_size)
				{
					// Find offset relative to the array
					int array_offset = array_variable.get_variable_address() + (i*4);

					// Allocate storage
					context.allocate_stack();
					int stack_pointer = context.get_stack_pointer();
					std::string init_register = "$8";

					(*expression_vector)[i]->compile(dst, context);

					context.deallocate_stack();

					// Output assembly instructions for loading and storage
					context.load_register(dst, init_register, stack_pointer);
					context.output_store_operation(dst, declarator_type.get_variable_type(), init_register, "$30", array_offset);
				}
				else
				{
					// Find offset relative to the array
					int array_offset = array_variable.get_variable_address() + (i*4);
					context.output_store_operation(dst, declarator_type.get_variable_type(), "$0", "$30", array_offset);
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

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override
		{
			initialisation_declarator->compile_declaration_initialisation(dst, context, declarator_type, initialisation_expressions, pointer_capable);
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
				int array_offset = array_variable.get_variable_address() + (i*4);

				// Allocate storage
				context.allocate_stack();
				int stack_pointer = context.get_stack_pointer();
				std::string init_register = "$8";

				(*initialisation_vector)[i]->compile(dst, context);

				context.deallocate_stack();

				// Output assembly instructions for loading and storage
				context.load_register(dst, init_register, stack_pointer);
				context.output_store_operation(dst, array_variable.get_variable_type(), init_register, "$30", array_offset);
			}
		}

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override
		{
			initialisation_declarator->compile_declaration_array_initialisation(dst, context, declarator_type, initialisation_vector);
		}
};

/* ------------------------------------						    Declaration Class					------------------------------------ */

class Declaration : public External_Declaration
{
	private:
		type_definition TYPE;
		bool pointer_capable;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(type_definition _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: TYPE(_TYPE), pointer_capable(_TYPE.get_pointer_capability()), declaration_list(_declaration_list) { }

		virtual std::string get_parameter() { return (*declaration_list)[0]->get_variable_name(); }

		type get_type() { return TYPE.get_variable_type(); }

		bool get_pointer_capable() {return pointer_capable; }

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			if (declaration_list != NULL)
			{				
				for (int i = 0; i < declaration_list->size(); i++)
				{					
					Declarator* temp_declarator = declaration_list->at(i);

					(*temp_declarator).compile_declaration(dst, context, TYPE, pointer_capable);					
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
		type_definition* TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement* statements;


	public:
		Function_Definition (type_definition* _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement* _statements) 
		: TYPE(_TYPE), ID(_ID), parameter_list(_parameter_list), statements(_statements) {}

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

			dst << "\t" << "sw"    << "\t" << "\t" << "$31,"	   << "-4"  << "($29)" << std::endl;
			dst << "\t" << "sw"    << "\t" << "\t" << "$30,"	   << "-8" << "($29)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$29,$29,"   		   << "-8" << std::endl; 
			dst << "\t" << "move"  << "\t" << "$30,$29"    		   << std::endl;

			dst << std::endl;
			dst << "# ------------ Program Assembly ------------ #" << std::endl;
			dst << std::endl;

			// Function arguments
			if(parameter_list != NULL)
			{
				int argument_stack_pointer = 4; // Set to 4 to ANSI spec

				std::string argument_integer_registers[4]  = {"$4", "$5", "$6", "$7"};
				std::string argument_float_registers[4]    = {"$f12", "$f14"};
				std::string argument_double_registers[4]   = {"$f12", "$f13", "$f14", "$f15"};

				// Check if parameters can fit into four argument register
				// https://stackoverflow.com/questions/2298838/mips-function-call-with-more-than-four-arguments

				for(int i = 0; i < parameter_list->size(); i++)
				{
					argument_stack_pointer += 4;
					type argument_type = (*parameter_list)[i]->get_type();

					if ((argument_type == FLOAT) && (i < 2)) // Float can only store two arguments
					{
						context.store_float_register(dst, argument_float_registers[i], argument_stack_pointer);
					}
					else if ((argument_type == DOUBLE) && (i < 2))
					{
						// Conforms to GodBolt format for allocating double registers
						int register_index = (i + 1)*2;

						argument_stack_pointer += 4;
						context.store_float_register(dst, argument_double_registers[register_index - 2], argument_stack_pointer);
						
						argument_stack_pointer -= 4;
						context.store_float_register(dst, argument_double_registers[register_index - 1], argument_stack_pointer);

						argument_stack_pointer += 4;
					}
					else if (((argument_type == FLOAT) && (i < 4)) || ((argument_type == INT || argument_type == UNSIGNED_INT) && (i < 4)))
					{
						if (argument_type == FLOAT)
						{
							context.output_store_operation(dst, argument_type, argument_integer_registers[i], "$30", argument_stack_pointer);	
						}
						else
						{
							context.output_store_operation(dst, argument_type, argument_integer_registers[i], "$30", argument_stack_pointer);	
						}
					}

					context.make_new_argument((*parameter_list)[i]->get_parameter(), (*parameter_list)[i]->get_type(), NORMAL, argument_stack_pointer, (*parameter_list)[i]->get_pointer_capable());
				}

			}
			
			// Function body
			if(statements != NULL)
			{ 
				statements->compile(dst, context);
			}
			
			// Godbolt format for main functions
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
			dst << "\t" << "addiu" << "\t" << "$29, $29," << "8" << std::endl; 
			dst << "\t" << "lw"    << "\t" << "\t" << "$30," << "-8" << "($29)" << std::endl;
			dst << "\t" << "lw"    << "\t" << "\t" << "$31," << "-4" << "($29)" << std::endl;
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

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override
		{ context.new_variable(function_name, declarator_type, FUNCTION); }
};

/* ------------------------------------						    Enumeration Class					------------------------------------ */

// https://stackoverflow.com/questions/8597426/enum-type-check-in-c-gcc
// https://stackoverflow.com/questions/62437717/how-does-c-compiler-treat-enum

class Initialisation_Enum_Declarator : public Declarator 
{
	// Expressions like "int a = 4 + 7 + b"
	private: 
		Declarator* initialisation_declarator;
		Expression* initialisation_expressions = NULL;

	public: 
		Initialisation_Enum_Declarator(Declarator* _initialisation_declarator, Expression* _initialisation_expressions)
		: initialisation_declarator(_initialisation_declarator), initialisation_expressions(_initialisation_expressions) {}

		Initialisation_Enum_Declarator(Declarator* _initialisation_declarator)
		: initialisation_declarator(_initialisation_declarator) {}

		virtual void compile_declaration(std::ostream &dst, Context& context, type_definition declarator_type, bool pointer_capable) const override
		{ initialisation_declarator->compile_declaration_initialisation(dst, context, declarator_type, initialisation_expressions, false); }
};

#endif