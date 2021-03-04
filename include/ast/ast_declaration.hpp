#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP


#include <iomanip>

#include "ast_expression.hpp"

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

/* ------------------------------------						    Declaration Class					------------------------------------ */

class External_Declaration : public Node {};

/* ------------------------------------						     Declarator Class					------------------------------------ */

class Declarator : public External_Declaration 
{
	public:
		virtual void compile_declaration(std::ostream &dst, Context& context, type declarator_type) const {}
		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const {}
};

class Variable_Declarator : public Declarator
{
	private:
		std::string variable_name;

	public:
		Variable_Declarator(std::string _variable_name) : variable_name(_variable_name) {}

		// Print MIPS

		virtual void compile(std::ostream& dst, Context& context) const override 
		{
			// Get necessary information
			variable compile_variable = context.get_variable(variable_name);
			type variable_type = compile_variable.get_variable_type();
			context_scope variable_scope = compile_variable.get_variable_scope();
			int variable_offset = 8 * compile_variable.get_variable_address();

			// Print MIPS
			context.output_load_operation(dst, variable_type, "0", "fp", variable_offset);
		}

		virtual void compile_declaration_initialisation(std::ostream &dst, Context& context, type declarator_type, Expression* expressions) const override 
		{
			// Get necessary information
			variable compile_variable = context.get_variable(variable_name);

			// Allocate memory
			context.allocate_stack();
			int frame_pointer = context.get_frame_pointer();

			expressions->compile(dst, context);

			context.deallocate_stack();
			context.store_register(dst, "v0", frame_pointer);
			context.output_load_operation(dst, declarator_type, "v0", "fp", compile_variable.get_variable_address()*8);
		}
};

class Initialisation_Variable_Declarator : public Declarator 
{
	// Expressions like a = 4 + 7 + b

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

/* ------------------------------------						  Declaration Class					------------------------------------ */

class Declaration : public External_Declaration
{
	private:
		std::string TYPE;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: TYPE(_TYPE), declaration_list(_declaration_list) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			if (declaration_list != NULL)
			{				
				for (int i = 0; i < declaration_list->size(); i++)
				{
					Declarator* temp_declarator = declaration_list->at(i);
					(*temp_declarator).compile(dst, context);
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

Note that stack grows downward i.e. as we push items onto the stack, the address decreases. The relative value of the stack 
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

			context.expand_variable_scope();
			context.set_LOCAL();
			
			/* -------------------------------- 		   Opening directives 			-------------------------------- */
			dst << "\t" << ".text"  << std::endl;
			dst << "\t" << ".globl" << "\t" << ID << std::endl;
			dst << "\t" << ".ent"   << "\t" << ID << std::endl;
			dst << "\t" << ".type"  << "\t" << ID <<", @function" << std::endl;

			/* -------------------------------- 	 			Function	 			-------------------------------- */
			dst <<  ID  << ":" << std::endl;
			dst << "\t" << ".set" << "\t" << "noreorder" << std::endl;
			dst << "\t" << ".set" << "\t" << "nomacro" 	<< std::endl;
			
			// Allocate stack (TODO: Revision required)
			dst << "\t" << "addiu" << "\t" << "$sp,$sp,-"  << "8" << std::endl; 
			dst << "\t" << "sw"    << "\t" << "$fp,"	   << "8" << "($sp)" <<std::endl;
			dst << "\t" << "move"  << "\t" << "$fp,$sp"    << std::endl;

			// Function parameters
			if(parameter_list != NULL)
			{

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
			else // Empty function
			{
				dst << "\t" << "nop" << "\t" << std::endl;
			}
			
			// Deallocate stack
			dst << "\t" << "move"  << "\t" << "$sp, $fp"  << std::endl; 
        	dst << "\t" << "lw"    << "\t" << "$fp," << "8" << "($sp)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$sp, $sp," << "8" << std::endl; 
			dst << "\t" << "j" 	   << "\t" << "$ra"  << std::endl;
			dst << "\t" << "nop" << "\t" << std::endl;
			dst << std::endl;

			/* -------------------------------- 		    Closing directives 			-------------------------------- */
			dst << "\t" << ".end" << "\t" << ID << std::endl;
			dst << "\t" << ".set" << "\t" << "macro" << std::endl;
			dst << "\t" << ".set" << "\t" << "reorder"  << std::endl;


			context.shrink_variable_scope();
			context.set_GLOBAL();
		}
};


#endif