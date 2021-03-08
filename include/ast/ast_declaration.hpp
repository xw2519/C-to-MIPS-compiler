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
		virtual std::string get_variable_name() { }

		virtual void compile_declaration(std::ostream &dst, Context& context, type declarator_type) const {}
		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const {}
};

class Variable_Declarator : public Declarator
{
	private:
		std::string variable_name;

	public:
		Variable_Declarator(std::string _variable_name) : variable_name(_variable_name) {}

		// Parameter functions
		virtual std::string get_variable_name() { return variable_name; }

		// Print MIPS
		virtual void compile_declaration(std::ostream& dst, Context& context, type declaration_type) const override 
		{
			// Get necessary information
			variable compile_variable = context.new_variable(variable_name, INT, NORMAL);

			type variable_type = compile_variable.get_variable_type();
			int frame_pointer_1 = compile_variable.get_variable_address();

			// Print MIPS
			context.output_store_operation(dst, variable_type, "0", "fp", frame_pointer_1);
		}

		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const override 
		{
			variable declared_variable = context.new_variable(variable_name, declarator_type, NORMAL);

			if(context.get_context_scope() == LOCAL)
			{
				// Allocate temporary registers
				context.allocate_stack();
				int frame_pointer_1 = context.get_frame_pointer();
				std::string temp_register_1 = "t0";
				
				expressions->compile(dst, context);

				// Deallocate
				context.deallocate_stack();

				context.load_register(dst, temp_register_1, frame_pointer_1);
				context.output_store_operation(dst, declarator_type, temp_register_1, "fp", declared_variable.get_variable_address());
			}
		}
};

// https://stackoverflow.com/questions/54721000/c-array-indexing-in-mips-assembly

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

		// Print MIPS
		virtual void compile_declaration(std::ostream& dst, Context& context, type declaration_type) const override 
		{
			// Get necessary information
			variable array_variable = context.new_variable(variable_name, INT, ARRAY);
			
			int frame_pointer_1 = array_variable.get_variable_address();
			int array_frame_pointer = 0;

			for(int i = 0; i < array_size; i++)
			{
				array_frame_pointer =  frame_pointer_1 - (i*8);
				// Print MIPS
				context.output_store_operation(dst, INT, "0", "fp", array_frame_pointer);
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

/* ------------------------------------						    Declaration Class					------------------------------------ */

class Declaration : public External_Declaration
{
	private:
		type TYPE;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: TYPE(convert_type(_TYPE)), declaration_list(_declaration_list) {}

		virtual std::string get_parameter()
		{
			return (*declaration_list)[0]->get_variable_name();
		}

		type convert_type(std::string TYPE)
		{
			if(TYPE == "int") { return INT; }
			else if(TYPE == "void") { return VOID; } 
		}

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

class Function_Definition : public External_Declaration // Very basic
{
	private:
		std::string TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement* statements;

	public:
		Function_Definition (std::string _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement *_statements) 
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

			dst << "\t" << "sw"    << "\t" << "\t" << "$31,"	   << "-8"  << "($sp)" << std::endl;
			dst << "\t" << "sw"    << "\t" << "\t" << "$fp,"	   << "-16" << "($sp)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$sp,$sp,"   << "-16" << std::endl; 
			dst << "\t" << "move"  << "\t" << "$fp,$sp"    << std::endl;

			dst << std::endl;
			dst << "# ------------ Program Assembly ------------ #" << std::endl;
			dst << std::endl;

			// Function parameters
			if(parameter_list != NULL) // Handles 4 argument for now
			{
				int argument_frame_pointer = 8; // Set to 8 to prevent stack frame clash

				std::string argument_registers[4]  = {"a0", "a1", "a2", "a3"};

				for(int i = 0; i < parameter_list->size(); i++)
				{
					argument_frame_pointer += 8;

					context.store_register(dst, argument_registers[i], argument_frame_pointer);
					context.make_new_argument((*parameter_list)[i]->get_parameter(), INT, NORMAL, argument_frame_pointer);
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

			dst << "\t" << "move"  << "\t" << "$sp, $fp"  << std::endl; 
			dst << "\t" << "addiu" << "\t" << "$sp, $sp," << "16" << std::endl; 
			dst << "\t" << "lw"    << "\t" << "\t" << "$fp," << "-16" << "($sp)" << std::endl;
			dst << "\t" << "lw"    << "\t" << "\t" << "$ra," << "-8" << "($sp)" << std::endl;
			dst << "\t" << "j" 	   << "\t" << "\t" << "$31"  << std::endl;

			dst << std::endl;
			dst << "# ------------ Closing directives ------------ #" << std::endl;
			dst << std::endl;
			dst << "\t" << ".end" << "\t" << ID << std::endl;

			context.shrink_context_scope();
			context.set_GLOBAL();
		}
};


#endif