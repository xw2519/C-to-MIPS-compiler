#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

#include <cmath>

#include "ast_variable.hpp"

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

/* ------------------------------------ 			  				Typedef variables		 					------------------------------------ */

typedef std::map<std::string, variable*> type_mapping;

/* ------------------------------------								Context Functions							------------------------------------ */
struct Context
{
	private:
		type_mapping* context_tracker = new type_mapping();

		// Context scope
		std::stack <type_mapping*> context_scope_stack_tracker;
		std::stack <int> context_scope_frame_pointer;

		// Trackers and counters
		int frame_pointer = 0;
		int register_counter = 0;
		
		context_scope scope_tracker = GLOBAL; // Set to global by default 

	public:
		/* ------------------------------------						     Scope Functions						------------------------------------ */

		void set_LOCAL()  { scope_tracker = LOCAL; }

		void set_GLOBAL() { scope_tracker = GLOBAL; }

		void expand_variable_scope()
		{
			context_scope_stack_tracker.push(context_tracker);
			context_tracker = new type_mapping(*context_tracker);
			context_scope_frame_pointer.push(frame_pointer);
		}	

		void shrink_variable_scope()
		{
			frame_pointer = context_scope_frame_pointer.top();
			
			delete context_tracker;
			context_tracker = context_scope_stack_tracker.top();

			context_scope_frame_pointer.pop();
			context_scope_stack_tracker.pop();
		}

		/* ------------------------------------						    Stack frame Functions					------------------------------------ */

		void allocate_stack() 
		{
			// Update trackers
			register_counter++;
			frame_pointer -= 8;
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


		/* ------------------------------------						     Register Functions						------------------------------------ */

		void load_register(std::ostream& dst, std::string register_name, int memory_location)	
		{
			dst << "\t" << "lw" << "\t" << "$" << register_name << "," << memory_location << "($fp)" << std::endl;
		}

		void store_register(std::ostream& dst, std::string register_name, int memory_location)	
		{
			dst << "\t" << "sw" << "\t" << "$" << register_name << "," << memory_location << "($fp)" << std::endl;
		}

		void output_load_operation(std::ostream& dst, type load_type, std::string register_1, std::string register_2, int frame_offset)
		{
			dst << "\t" << "sw" << "\t" << "$" << register_1 << "," << frame_offset << "($" << register_2 << ")" << std::endl;
		}

		// Float operations not done yet

		/* ------------------------------------						  Context Variable Functions				------------------------------------ */
		
		variable new_variable(std::string variable_name, type variable_type, declaration_type variable_declaration_type)
		{
			frame_pointer -= (1*8);

			(*context_tracker)[variable_name] = new variable(frame_pointer, LOCAL, variable_declaration_type, variable_type);

			return *((*context_tracker)[variable_name]);
		}

		variable get_variable(std::string variable_name)
		{
			// Return number of occurrences of a variable
			if((*context_tracker).count(variable_name))
			{
				return *((*context_tracker)[variable_name]);
			}
		}


};




#endif