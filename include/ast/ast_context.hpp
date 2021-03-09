#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <cmath>
#include <regex>

#include <memory>

#include <fstream>
#include <stdexcept>
#include <initializer_list>

/*

$0	        $zero	    Hard-wired to 0 										(Not used for normal use)
$1	        $at	        Reserved for pseudo-instructions						(Not used for normal use)
$2  - $3	$v0,  $v1	Return values from functions							(Not used for normal use)
$4  - $7	$a0 - $a3	Procedure arguments - not preserved by subprograms		(Not used for normal use)
$8  - $15	$t0 - $t7	Temporary registers, not preserved by subprograms
$16 - $23	$s0 - $s7	Saved registers, preserved by subprograms
$24 - $25	$t8 - $t9	More temporary registers, not preserved by subprograms
$26 - $27	$k0 - $k1	Reserved for kernel. Do not use.						(Not used for normal use)
$28	        $gp	        Global Area Pointer (base of global data segment)		(Not used for normal use)
$29	        $sp	        Stack Pointer											(Not used for normal use)
$30	        $fp	        Frame Pointer											(Not used for normal use)
$31	        $ra	        Return Address											(Not used for normal use)

------------------------------------------------------------------------------------------------------------

$4  - $7				Procedure arguments
$8 	- $15				Temporary registers that can be overwritten by called procedures
$16 - $23				Temporary registers but cannot be overwritten by procedures
$24	- $25				Temporary registers

*/

/* ------------------------------------ 			  			     Custom definitions			 				------------------------------------ */

enum declaration_type
{
	NORMAL,
	FUNCTION,
	ARRAY,
	STRUCT
};

enum context_scope
{
	GLOBAL, 
	LOCAL
};

enum type
{
	VOID,
	INT,
	NONE
};

/* ------------------------------------ 			      			  Variable class		 					------------------------------------ */
// Handles the properties of the variable including:
// 	- Context scope
//	- Variable type
//	- Declaration 
//	- Memory address

class variable
{
	private: 
		int variable_address;
		context_scope variable_scope = GLOBAL;
		declaration_type variable_declaration;
		type variable_type;

	public:
		variable (int _variable_address, context_scope _variable_scope, declaration_type _variable_declaration, type _variable_type) 
		: variable_address(_variable_address), variable_scope(_variable_scope), variable_declaration(_variable_declaration), variable_type(_variable_type) {}

		// Get attributes
		int get_variable_address() { return variable_address; }
		context_scope get_variable_scope() { return variable_scope; }
		type get_variable_type() { return variable_type; }
		declaration_type get_declaration_type() { return variable_declaration; }
};


/* ------------------------------------ 			      			    Type class		 						------------------------------------ */
// Handles the properties of the variable types including:
// 	- The type of variable e.g. int, void
//	- Parameters or values attached with the variable type e.g. arrays
//	- Array tracker variables 

class type_definition
{
	private: 
		// Variable type set to NONE by default
		type variable_type = NONE;

		// Array trackers (default set to 0)
		int array_pointer_tracker = 0;
		int array_size_tracker = 0;

	public:
		type_definition(type _variable_type) : variable_type(_variable_type) {}

		// Variable type oeprators
		type get_variable_type() { return variable_type; }

		// Array trackers
		int get_array_pointer() { return array_pointer_tracker; }
		int get_array_size() { return array_size_tracker; }
		
		void increase_array_pointer() { array_pointer_tracker += 1;}

		void decrease_array_pointer() { array_pointer_tracker -= 1;}

};


/* ------------------------------------								 Context Functions							------------------------------------ */

static context_scope scope_tracker;
typedef std::map<std::string, variable*> type_mapping;

class Context
{
	private:
		type_mapping* context_tracker = new type_mapping();

		// Context scope
		std::stack <type_mapping*> context_scope_stack_tracker;
		std::stack <int> context_scope_frame_pointer;

		// Trackers and counters
		int frame_pointer = 0;
		int register_counter = 0;
		
		// Jump definitions 
		int label_counter = 0;
		std::string main_function_return_label; 

	public:
		/* ------------------------------------						    Return Functions						------------------------------------ */
		
		std::string get_function_return_label() { return main_function_return_label; }

		void set_main_return_label(std::string return_label)
		{
			label_counter++;
			main_function_return_label = return_label + "_" + std::to_string(label_counter);
		}

		std::string make_label(std::string return_label)
		{
			label_counter++;
			return return_label + "_" + std::to_string(label_counter);
		}

		/* ------------------------------------						     Scope Functions						------------------------------------ */

		context_scope get_context_scope() { return scope_tracker; }

		void set_LOCAL()  { scope_tracker = LOCAL; }

		void set_GLOBAL() { scope_tracker = GLOBAL; }

		void expand_context_scope()
		{
			context_scope_stack_tracker.push(context_tracker);
			context_tracker = new type_mapping(*context_tracker);
			context_scope_frame_pointer.push(frame_pointer);
		}

		void shrink_context_scope()
		{
			frame_pointer = context_scope_frame_pointer.top();

			delete context_tracker;
			context_tracker = context_scope_stack_tracker.top();

			context_scope_frame_pointer.pop();
			context_scope_stack_tracker.pop();
		}

		/* ------------------------------------						   Stack frame Functions					------------------------------------ */

		void allocate_stack()
		{
			// Update trackers
			register_counter++;
			frame_pointer -= 8;

			// std::cerr << "FP update" << frame_pointer << std::endl;
		}

		void deallocate_stack()
		{
			// Only deallocate if there are registers already allocated
			if (register_counter != 0)
			{
				frame_pointer += 8;
				register_counter--;
			}
		}

		int get_frame_pointer() { return frame_pointer; }


		/* ------------------------------------						    Register Functions						------------------------------------ */

		void load_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "lw" << "\t" << "\t" << "$" << register_name << "," << memory_location << "($fp)" << std::endl;
		}

		void store_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "sw" << "\t" << "\t" << "$" << register_name << "," << memory_location << "($fp)" << std::endl;
		}

		void output_load_operation(std::ostream& dst, type load_type, std::string register_1, std::string register_2, int frame_offset)
		{
			dst << "\t" << "lw" << "\t" << "\t" << "$" << register_1 << "," << frame_offset << "($" << register_2 << ")" << std::endl;
		}

		void output_store_operation(std::ostream& dst, type load_type, std::string register_1, std::string register_2, int frame_offset)
		{
			dst << "\t" << "sw" << "\t" << "\t" << "$" << register_1 << "," << frame_offset << "($" << register_2 << ")" << std::endl;
		}

		// Float operations not done yet

		/* ------------------------------------						  	  Variable Functions					------------------------------------ */
		
		variable new_variable(std::string variable_name, type variable_type, declaration_type variable_declaration_type, int variable_size = 1)
		{
			// Set of multiples of 8
			if(scope_tracker == LOCAL)
			{
				frame_pointer -= variable_size*(8);
			}

			(*context_tracker)[variable_name] = new variable(frame_pointer, scope_tracker, variable_declaration_type, variable_type);

			return *((*context_tracker)[variable_name]);
		}

		variable get_variable(std::string variable_name)
		{
			// Return variable
			if((*context_tracker).count(variable_name))
			{
				return *((*context_tracker)[variable_name]);
			}
		}

		/* ------------------------------------						  	  Argument Functions					------------------------------------ */

		void make_new_argument(std::string argument_name, type argument_type, declaration_type argument_declaration, int argument_address)
		{ 
			(*context_tracker)[argument_name] = new variable(argument_address, LOCAL, argument_declaration, argument_type);
		}

};


/* ------------------------------------ 			      			   Node class		 			 			------------------------------------ */

class Node
{
	public:
		virtual ~Node () {}

		virtual void compile(std::ostream& dst, Context& context) const 
		{
			std::cerr << "Ast_node.hpp: 'compile' not implemented" << std::endl;
		}
};

typedef const Node* Node_Ptr;



/* ------------------------------------				  Functions for code generation				  ------------------------------------ */

//std::string get_float_label(double value)                                   // get label of  ".word <float value> directive"
//std::string get_string_label(std::String value)                             // get label of  ".ascii <string literal/000> directive"
//std::string make_label()                                                    // generate unique label and return as string (Implemented)

//std::string next_reg(std::string someReg)                                   // return name of next register
//std::string alloc_reg(ExpressionEnum type, int amount=1)                    // return name of free register, mark it and following amount as occupied
//void dealloc_reg(std::string someReg, int amount=1)                         // free register and following amount of registers

//bool check_global(std::string identifier)                                   // return true if identifier is global

//ExpressionEnum get_type(std::string identifier)                             // return type of identifier
//ExpressionEnum get_type_pointed(std::string identifier)                     // return type pointed to by identifier
//ExpressionEnum get_type_member(std::string identifier, std::string member)  // return type of member of struct

//std::string id_to_addr(std::string identifier)                              // return address, relative to $fp, of identifier
//std::string member_to_addr(std::string identifier, std::string member)      // return address of member, relative to beginning of struct

//int size_of_pointed(std::string identifier)                                 // return size of value pointed to by identifier, in words
//int size_of_member(std::string identifier, std::string member);             // return size of member, in words
//int size_of(std::string identifier)                                         // return size of identifier in words
//int size_of(Declaration* declr)                                             // return size of declaration







#endif
