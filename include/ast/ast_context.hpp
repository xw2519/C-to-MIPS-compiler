#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

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
		int frame_pointer;
		int register_counter;

		context_scope scope_tracker = GLOBAL; // Set to global by default

	public:
		/* ------------------------------------						     Scope Functions						------------------------------------ */

		void set_LOCAL()
		{
			scope_tracker = LOCAL;
		}

		void set_GLOBAL()
		{
			scope_tracker = GLOBAL;
		}

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
			frame_pointer -= 4;
		}

		void deallocate_stack()
		{
			// Only deallocate if there are registers already allocated
			if (register_counter != 0)
			{
				frame_pointer += 4;
				register_counter--;
			}
		}

		int get_frame_pointer()
		{
		return frame_pointer;
		}


		/* ------------------------------------						     Register Functions						------------------------------------ */

		void load_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "lw $" << register_name << "," << memory_location << "($fp)" <<std::endl;
		}

		void store_register(std::ostream& dst, std::string register_name, int memory_location)
		{
			dst << "sw $" << register_name << "," << memory_location << "($fp)" <<std::endl;
		}

		// Float operations not done yet

		/* ------------------------------------						  Context Variable Functions				------------------------------------ */

		variable add_variable()

    /* ------------------------------------				  Functions for code generation				  ------------------------------------ */

    std::string get_float_label(double value)                                   // get label of  ".word <float value> directive"
    std::string get_string_label(std::String value)                             // get label of  ".ascii <string literal/000> directive"
    std::string make_label()                                                    // generate unique label and return as string

    std::string next_reg(std::string someReg)                                   // return name of next register
    std::string alloc_reg(ExpressionEnum type, int amount=1)                    // return name of free register, mark it and following amount as occupied
    void dealloc_reg(std::string someReg, int amount=1)                         // free register and following amount of registers

    bool check_global(std::string identifier)                                   // return true if identifier is global

    ExpressionEnum get_type(std::string identifier)                             // return type of identifier
    ExpressionEnum get_type_pointed(std::string identifier)                     // return type pointed to by identifier
    ExpressionEnum get_type_member(std::string identifier, std::string member)  // return type of member of struct

    std::string id_to_addr(std::string identifier)                              // return address, relative to $fp, of identifier
    std::string member_to_addr(std::string identifier, std::string member)      // return address of member, relative to beginning of struct

    int size_of_pointed(std::string identifier)                                 // return size of value pointed to by identifier, in words
    int size_of_member(std::string identifier, std::string member);             // return size of member, in words
    int size_of(std::string identifier)                                         // return size of identifier in words
    int size_of(Declaration* declr)                                             // return size of declaration


};


#endif
