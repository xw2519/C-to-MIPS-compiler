#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <regex>
#include <stack>
#include <cassert>
#include <deque>

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

/* ------------------------------------ 			  			   Forward declarations			 				------------------------------------ */

class Expression;
class Statement;

/* ------------------------------------ 			  			    Custom definitions			 				------------------------------------ */

enum declaration_type
{
	NORMAL,
	FUNCTION,
	ARRAY,
	POINTER,
	STRUCT
};

enum context_scope
{
	GLOBAL, 
	LOCAL
};

enum storage_type
{
	WORD,
	HALF,
	BYTE
};	

enum type
{
	VOID,
	INT,
	FLOAT,
	STRING,
	CHAR,
	DOUBLE,
	UNSIGNED_INT,
	NONE
};

/* ------------------------------------ 			      			    Type class		 						------------------------------------ */
// Handles the properties of the variable, pointer, array types including:
// 	- The type of variable e.g. int, void
//	- Parameters or values attached with the variable type e.g. arrays
//	- Array tracker variables 
//	- Pointer capability

class type_definition
{
	private: 
		// Variable type set to NONE by default
		type variable_type;
		bool pointer_capable=0;

		// Trackers (default set to 0)
		unsigned int pointer_tracker = 0;
		unsigned int array_tracker = 0;

	public:
		type_definition(type _variable_type) : variable_type(_variable_type) {}
		type_definition(std::string _pointer_flag, type_definition _pointer_type) 
		{
			
			if (_pointer_flag == "POINTER")
			{
				pointer_capable = 1;
				variable_type = _pointer_type.get_variable_type();
				pointer_tracker++;
			}
		}

		// Variable type operators
		type get_variable_type() { return variable_type; }
		
		// Trackers
		unsigned int get_array_tracker() { return array_tracker; }
		unsigned int get_pointer_tracker() { return pointer_tracker; }

		void increment_array_tracker() { array_tracker++; };
		void increment_pointer_tracker() { pointer_tracker++; };

		void decrement_array_tracker() { array_tracker--; };
		void decrement_pointer_tracker() 
		{ 
			if (pointer_tracker != 0) { array_tracker--; }
			else { pointer_tracker--; }
		};

		// Pointer operators
		void set_pointer_capability() { pointer_capable = 1; }
		bool get_pointer_capability() { return pointer_capable; }
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
		type_definition variable_type;
		bool pointer_capable=0;

	public:
		variable (int _variable_address, context_scope _variable_scope, declaration_type _variable_declaration, type_definition _variable_type) 
		: variable_address(_variable_address), variable_scope(_variable_scope), variable_declaration(_variable_declaration), variable_type(_variable_type) {}

		variable (int _variable_address, context_scope _variable_scope, declaration_type _variable_declaration, type_definition _variable_type, bool _pointer_capable) 
		: variable_address(_variable_address), variable_scope(_variable_scope), variable_declaration(_variable_declaration), variable_type(_variable_type), pointer_capable(_pointer_capable) {}

		// Get attributes
		int get_variable_address() { return variable_address; }  					// return address of member, relative to beginning of struct
		context_scope get_variable_scope() { return variable_scope; }				// return scope of member, relative to beginning of struct
		type get_variable_type() { return variable_type.get_variable_type(); }		
		type_definition get_type_definition() { return variable_type; }					
		declaration_type get_declaration_type() { return variable_declaration; }
		bool get_pointer_capability() { return pointer_capable; }

		// Variable functions
		std::string get_storage_type()
		{
			if (variable_type.get_variable_type() == INT || variable_type.get_variable_type() == FLOAT || variable_type.get_variable_type() == DOUBLE) { return "word"; }
			else { return "word"; }
		}	
};

/* ------------------------------------								 Context Functions							------------------------------------ */

static context_scope scope_tracker;
typedef std::map<std::string, variable*> type_mapping;
typedef std::map<int, variable*> variable_address_mapping;

class Context
{
	private:
		type_mapping* context_tracker = new type_mapping();
		variable_address_mapping* address_tracker = new variable_address_mapping();

		// Context scope
		std::stack<type_mapping*> context_scope_stack_tracker;
		std::stack<int> context_scope_frame_pointer;

		// Trackers and counters
		int stack_pointer = 0;
		int register_counter = 0;
		
		// Jump definitions 
		int label_counter = 0;
		std::string main_function_return_label; 

		// Break and Continue 
		std::stack<std::string> break_stack;
		std::stack<std::string> continue_stack;

		// Switch case structure
		std::deque<Expression*> switch_statements_tracker;
		std::deque<std::string> switch_label_tracker;

		// Labelled variables and pointers
		std::map<std::string, std::string> label_variables;
		std::map<std::string, std::string> label_declarations;

	public:
		/* ------------------------------------ 			      	  Function call Functions					------------------------------------ */

		bool check_function_declared(std::string function_name)
		{
			if((*context_tracker).count(function_name)) { return true; }
			else { return false; }
		}

		/* ------------------------------------				 Labeled variables and pointers Functions			------------------------------------ */
		
		void add_label_variable(std::string label, std::string variable)
		{
			label_variables[variable] = label;
		};

		void add_label_declaration(std::string variable, std::string declaration)
		{
			label_declarations[variable] = declaration;
		};

		/* ------------------------------------					   		 Switch Functions						------------------------------------ */

		void add_case_statements(Expression* case_statement, std::string case_label) 
		{ 
			switch_statements_tracker.push_back(case_statement);
			switch_label_tracker.push_back(case_label);
		}
		
		void add_case_label(std::string case_label) 
		{ 
			switch_label_tracker.push_back(case_label);
		}

		Expression* get_case_statement()
		{
			return switch_statements_tracker.front();
		}

		std::string get_case_label()
		{
			return switch_label_tracker.front();
		}

		int get_case_statement_size()
		{
			return switch_statements_tracker.size();
		}

		int get_case_label_size()
		{
			return switch_label_tracker.size();
		}

		void remove_case_statement() 
		{
			// Remove the first element of the vector
			switch_statements_tracker.pop_front();
		}

		void remove_label_statement() 
		{
			// Remove the first element of the vector
			switch_label_tracker.pop_front();
		}

		/* ------------------------------------					   Break and Continue Functions					------------------------------------ */

		void add_break_label(std::string break_label) { break_stack.push(break_label); }

		void add_continue_label(std::string continue_label) { continue_stack.push(continue_label); }

		std::stack<std::string> get_break_label() { return break_stack; }

		std::stack<std::string> get_continue_label() { return continue_stack; }

		void remove_break_label() { break_stack.pop(); }

		void remove_continue_label() { continue_stack.pop(); }

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
			context_scope_frame_pointer.push(stack_pointer);

			context_scope_stack_tracker.push(context_tracker);
			context_tracker = new type_mapping(*context_tracker);
		}

		void shrink_context_scope()
		{
			// Remove old context tracker
			delete context_tracker;

			stack_pointer = context_scope_frame_pointer.top();
			context_tracker = context_scope_stack_tracker.top();

			context_scope_frame_pointer.pop();
			context_scope_stack_tracker.pop();
		}

		/* ------------------------------------						   Stack frame Functions					------------------------------------ */

		void allocate_stack()
		{
			// Update trackers
			register_counter++;
			stack_pointer -= 4;

			// std::cerr << "FP update" << stack_pointer << std::endl;
		}

		void deallocate_stack()
		{
			// Only deallocate if there are registers already allocated
			if (register_counter != 0)
			{
				stack_pointer += 4;
				register_counter--;
			}
		}

		int get_stack_pointer() { return stack_pointer; }

		/* ------------------------------------						    Register Functions						------------------------------------ */

		void load_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "lw" << "\t" << "\t" << register_name << "," << memory_location << "($30)" << std::endl;
		}

		void store_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "sw" << "\t" << "\t" << register_name << "," << memory_location << "($30)" << std::endl;
		}

		void output_load_operation(std::ostream& dst, type load_type, std::string register_1, std::string register_2, int frame_offset)
		{
			dst << "\t" << "lw" << "\t" << "\t" << register_1 << "," << frame_offset << "(" << register_2 << ")" << std::endl;
		}

		void output_store_operation(std::ostream& dst, type load_type, std::string register_1, std::string register_2, int frame_offset)
		{
			dst << "\t" << "sw" << "\t" << "\t" << register_1 << "," << frame_offset << "(" << register_2 << ")" << std::endl;
		}

		/* ------------------------------------						  	Variable Functions						------------------------------------ */
		
		variable new_variable(std::string variable_name, type_definition variable_type, declaration_type variable_declaration_type, bool pointer_capable = 0, int variable_size = 1)
		{
			// Set of multiples of 4
			if (scope_tracker == LOCAL)
			{
				if (variable_declaration_type == ARRAY) { variable_type.decrement_pointer_tracker(); }
				stack_pointer -= variable_size*(4);
			}

			if (pointer_capable)
			{
				(*context_tracker)[variable_name] = new variable(stack_pointer, scope_tracker, variable_declaration_type, variable_type, pointer_capable);
				(*address_tracker)[stack_pointer] = new variable(stack_pointer, scope_tracker, variable_declaration_type, variable_type, pointer_capable);
			}
			else
			{
				(*address_tracker)[stack_pointer] = new variable(stack_pointer, scope_tracker, variable_declaration_type, variable_type, pointer_capable);
				(*context_tracker)[variable_name] = new variable(stack_pointer, scope_tracker, variable_declaration_type, variable_type);
			}		

			return *((*context_tracker)[variable_name]);
		}

		variable get_variable_address(int address)
		{
			// std::cerr<< address <<std::endl;
			// Return variable
			if((*address_tracker).count(address))
			{
				return *((*address_tracker)[address]);
			}
		}

		variable get_variable(std::string variable_name)
		{
			// Return variable
			if((*context_tracker).count(variable_name))
			{
				return *((*context_tracker)[variable_name]);
			}
		}

		bool get_pointer_capability(std::string variable_name)
		{
			return (*context_tracker)[variable_name]->get_pointer_capability();
		}

		void pointer_shift(std::ostream& dst, type_definition variable_type, int stack_pointer)
		{
			std::cerr<< variable_type.get_pointer_capability() <<std::endl;
			
			//std::cerr<< variable_type.get_variable_type() <<std::endl;

			if ((variable_type.get_pointer_capability()) && (variable_type.get_variable_type() == INT))
			{
				dst << "\t" << "sll" << "\t" << "\t" << "$2" << "," << "$2" << "," << 2 << std::endl;
				store_register(dst, "$2", stack_pointer);
			}
			
		}

		/* ------------------------------------						  	Argument Functions						------------------------------------ */

		void make_new_argument(std::string argument_name, type_definition argument_type, declaration_type argument_declaration, int argument_address, bool pointer_capable = 0)
		{ 
			(*context_tracker)[argument_name] = new variable(argument_address, LOCAL, argument_declaration, argument_type, pointer_capable);
		}

		/* ------------------------------------						      Float Functions						------------------------------------ */

		void shift_to_float_reg(std::ostream& dst, std::string int_register, std::string float_register)
		{
			dst << "\t" << "mtc1"  << "\t" << int_register << "," << float_register << std::endl;
		}

		void shift_from_float_reg(std::ostream& dst, std::string int_register, std::string float_register)
		{
			dst << "\t" << "mfc1"  << "\t" << int_register << "," << float_register << std::endl;
		}

		void load_float_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "lwc1" << "\t" << register_name << "," << memory_location << "($30)" << std::endl;
		}

		void store_float_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "\t" << "swc1" << "\t" << register_name << "," << memory_location << "($30)" << std::endl;
		}

};

/* ------------------------------------ 			      			    Node class		 			 			------------------------------------ */
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

#endif
