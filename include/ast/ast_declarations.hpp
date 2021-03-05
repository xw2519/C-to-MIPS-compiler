#ifndef DECLARATIONS
#define DECLARATIONS

#include "ast_node.hpp"
#include "ast_expression.hpp"


class Statement : public Node {};

class Program : public Node
{
	protected:
		Node_Ptr left;
		Node_Ptr right;

	private:

	public:
		Program(Node_Ptr _left, Node_Ptr _right) : left(_left), right(_right) {}

		virtual void print_MIPS(std::ostream &dst, Context& context) const override
		{
			left ->print_MIPS(dst, context);
			right->print_MIPS(dst, context);
		}
};


class External_Declaration : public Node {};

class Declarator : public External_Declaration
{
	private:
		std::string ID;
		Expression *initialisation_expression;

	public:

		Declarator (std::string _ID, Expression *_initialisation_expression = NULL)
		: ID(_ID), initialisation_expression(_initialisation_expression)
		{
			// Update tracker
			local_variable_counter++;
		}

		std::string getID()
		{
			return ID;
		}

		void print_MIPS(std::ostream &dst, Context& context) const override
		{

		}


};

class Declaration : public External_Declaration
{
	private:
		std::string TYPE;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL)
		: TYPE(_TYPE), declaration_list(_declaration_list) {}

		void print_MIPS(std::ostream &dst, Context& context) const override
		{
			if (declaration_list != NULL)
			{
				for (int i = 0; i < declaration_list->size(); i++)
				{
					Declarator* temp_declarator = declaration_list->at(i);
					(*temp_declarator).print_MIPS(dst, context);
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

		void print_MIPS (std::ostream &dst, Context &context) const override
		{

			/* -------------------------------- 		   Opening directives 			-------------------------------- */
			dst << "\t" << ".text"  << std::endl;
			dst << "\t" << ".globl" << "\t" << ID << std::endl;
			dst << "\t" << ".ent"   << "\t" << ID << std::endl;
			dst << "\t" << ".type"  << "\t" << ID <<", @function" << std::endl;

			/* -------------------------------- 	 			Function	 			-------------------------------- */
			dst << ID << ":" << std::endl;

			// Calculate stack frame required
			int stack_size = (local_variable_counter + global_variable_counter)*8 + (parameter_counter)*8; // To be revised

			// Allocate stack (TODO: Revision required)
			dst << "\t" << "addiu" << "\t" << "$sp,$sp,-"  << stack_size 	<< std::endl;
			dst << "\t" << "sw"    << "\t" << "$fp,"	   << stack_size - 4 << "($sp)" <<std::endl;
			dst << "\t" << "move"  << "\t" << "$fp,$sp"    << std::endl;

			// Function parameters
			if(parameter_list != NULL)
			{

			}

			// Function body
			if(statements != NULL)
			{
				statements->print_MIPS(dst, context);
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
        	dst << "\t" << "lw"    << "\t" << "$fp," << stack_size - 4 << "($sp)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$sp, $sp," << stack_size << std::endl;
			dst << "\t" << "j" 	   << "\t" << "$ra"  << std::endl;
			dst << "\t" << "nop" << "\t" << std::endl;
			dst << std::endl;

			/* -------------------------------- 		    Closing directives 			-------------------------------- */
			dst << "\t" << ".end"  << "\t" << ID << std::endl;
		}
};


#endif
