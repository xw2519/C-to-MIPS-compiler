#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP

#include "ast_expression.hpp"

#include <iomanip>

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


class Declarator : public Node
{
	private:
		std::string ID;
		Expression *initialisation_expression;

	public:
		std::string getID()
		{
			return ID;
		}

		Declarator (std::string _ID = "", Expression *initialisation_expression = NULL) 
		: ID(_ID), initialisation_expression(initialisation_expression) {}

		virtual void print_MIPS (std::ostream &dst, Context& context)
		{
			if (context.return_scope() == "global")
			{

			}
			else
			{
				context.store_variable(ID);

				dst << "sw $0," << context.find_variable_location(ID).first << "($fp)" << std::endl;

				if (initialisation_expression != NULL)
				{
					std::string tempReg_r = "v1";

					initialisation_expression->print_MIPS(dst, context);

					dst << "lw $v0," << context.decrease_frame_pointer() << "($fp)" << std::endl;

					dst << "sw $v0," << "," << context.find_variable_location(ID).first << "($fp)" << std::endl;

					context.increase_frame_pointer();
				}
			}
		}
};


class Declaration : public Node
{
	private:
		std::string type;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: type(_TYPE), declaration_list(_declaration_list) {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const override
		{
			if(declaration_list != NULL)
			{
				if(context.return_scope() == "local")
				{
					for(auto it = declaration_list->begin(); it != declaration_list->end(); it++)
					{
						(*it)->print_MIPS(dst, context);
					}
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

class Function_Definition : public Node // Very basic
{
	private:
		std::string TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement* statements;

	public:
		Function_Definition (std::string _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement *_statements) 
		: TYPE(_TYPE), ID(_ID), parameter_list(_parameter_list), statements(_statements) {}

		virtual void print_MIPS (std::ostream &dst, Context &context) const override
		{
			
			/* -------------------------------- 		   Opening directives 			-------------------------------- */
			dst << "\t" << ".text"  << std::endl;
			dst << "\t" << ".globl" << "\t" << ID << std::endl;
			dst << "\t" << ".ent"   << "\t" << ID << std::endl;
			dst << "\t" << ".type"  << "\t" << ID <<", @function" << std::endl;

			/* -------------------------------- 	 			Function	 			-------------------------------- */
			dst << ID << ":" << std::endl;

			// Allocate stack (TODO: Revision required)
			dst << "\t" << "addiu" << "\t" << "$sp,$sp," << "-8" << std::endl; 
			dst << "\t" << "sw"    << "\t" << "$fp,"	  << 4 << "($sp)" <<std::endl;
			dst << "\t" << "move"  << "\t" << "$fp,$sp"   << std::endl;

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
        	dst << "\t" << "lw"    << "\t" << "$fp," << 4 << "($sp)" << std::endl;
			dst << "\t" << "addiu" << "\t" << "$sp, $sp," << 8 << std::endl; 
			dst << "\t" << "j" 	   << "\t" << "$ra"  << std::endl;
			dst << std::endl;

			/* -------------------------------- 		    Closing directives 			-------------------------------- */
			dst << "\t" << ".end"  << "\t" << ID << std::endl;
		}
};


#endif