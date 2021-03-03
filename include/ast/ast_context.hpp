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

/* ------------------------------------ 			  				External variables		 					------------------------------------ */

static int parameter_counter;

static int local_variable_counter;
static int global_variable_counter;


/* ------------------------------------								Context Functions							------------------------------------ */
struct Context
{
	private:
		// Register availability tracker
		bool register_availability_tracker[32]; // Register $0 to $31

		// Parameter list storage for function arguments 
		std::vector <std::string> parameter_list;
		std::vector <std::string> return_value_list;

		// Variable (Local and Global)
		std::map <std::string, int> local_variables; 	// Name, Location
		std::map <std::string, int> global_variables; 	// Name, Location

		// Pointers
		int stack_pointer_tracker;
		int frame_pointer_tracker;


	
	public:
		~Context () {}

		Context () 
		{
			// Initialise trackers
			local_variable_counter = 0;
			global_variable_counter = 0;
			parameter_counter = 0;

			// Initialise the register availability tracker
			// TRUE : Available 
			// FALSE: Unavailable
			for (int i = 0; i < 32; i++) 
			{
				if (i <= 3 || i >= 26) 
				{
					register_availability_tracker[i] = false;
				}
				else
				{
					register_availability_tracker[i] = true;
				}
        	}

		}

		/* ------------------------------------						General Register Functions					------------------------------------ */

		std::vector<int> list_available_registers () // Return list of all available registers
		{
			std::vector<int> avaiable_registers;

			if (avaiable_registers.size() == 0) { throw std::runtime_error ("No available registers"); } 

			for (int i = 0; i < 31; i++) 
			{
				if (register_availability_tracker[i] == true) 
				{
					avaiable_registers.push_back(i);
				}
			}

			return avaiable_registers;     
    	}

		std::vector<int> list_unavailable_registers () // Return all unavailable registers
		{
			std::vector<int> unavaiable_registers;

			if (unavaiable_registers.size() == 0) { throw std::runtime_error ("No available registers"); } 

			for (int i = 0; i < 31; i++) 
			{
				if (register_availability_tracker[i] == true) 
				{
					unavaiable_registers.push_back(i);
				}
			}

			return unavaiable_registers;     
    	}

		void set_avaiable (int register_ID) 
		{
        	register_availability_tracker[register_ID] = true;
    	}

		void set_unavaiable (int register_ID) 
		{
        	register_availability_tracker[register_ID] = false;
    	}

		/* ------------------------------------					Temprorary Register Functions					------------------------------------ */

		std::vector<int> list_available_temprorary_registers () 
		{
			std::vector<int> available_temprorary_registers;

			for (int i = 8; i <= 15; i++) 
			{
				if (register_availability_tracker[i] == true) 
				{
					available_temprorary_registers.push_back(i);
				}
			}

			for (int i = 24; i <= 25; i++) 
			{
				if (register_availability_tracker[i] == true) 
				{
					available_temprorary_registers.push_back(i);
				}
        	}    

        	return available_temprorary_registers;     
    	}

		std::vector<int> list_unavailable_temprorary_registers () 
		{
			std::vector<int> unavailable_temprorary_registers;

			for (int i = 8; i <= 15; i++) 
			{
				if (register_availability_tracker[i] == false) 
				{
					unavailable_temprorary_registers.push_back(i);
				}
			}

			for (int i = 24; i <= 25; i++) 
			{
				if (register_availability_tracker[i] == false) 
				{
					unavailable_temprorary_registers.push_back(i);
				}
        	}    

        	return unavailable_temprorary_registers;     
    	}
	
		/* ------------------------------------					Constant Register Functions					------------------------------------ */

		std::vector<int> list_available_constant_registers () 
		{

    	}

		/* ------------------------------------				  Return Value Register Functions				------------------------------------ */

		std::vector<int> list_available_return_registers ()
		{
			std::vector<int> available_return_registers;

			for (int i = 2; i <= 3; i++) 
			{
				if (register_availability_tracker[i] == false) 
				{
					available_return_registers.push_back(i);
				}
        	}    

			return available_return_registers;
		}

		void set_return_value_register (std::string return_value)
		{
			if (return_value_list.size() > 2) { throw std::runtime_error("Exceeding stack boundaries"); }

			return_value_list.push_back(return_value);
    	}

		int find_return_location (std::string return_value)
		{
			for (int i = 0; i < return_value_list.size(); i++) // Search parameter list and return location of matching term
			{
				if(return_value == parameter_list[i]) { return i; }
			}

			return -1; // Return -1 if no matching term is found
    	}

		void clean_return_registers () 
		{
        	for (int i = 2; i <= 3; i++) { register_availability_tracker[i] = true; }

			parameter_list.clear();
    	}



		/* ------------------------------------					Parameter Register Functions				------------------------------------ */
		
		std::vector<int> list_available_parameter_registers () 
		{
			std::vector<int> available_parameter_register;

			for (int i = 4; i <= 7; i++) 
			{
				if (register_availability_tracker[i] == false) 
				{
					available_parameter_register.push_back(i);
				}
        	}    

        	return available_parameter_register;
    	}

		void set_parameter_register (std::string parameter)
		{
			if (parameter_list.size() > 4) { throw std::runtime_error("Exceeding stack boundaries"); }

			parameter_list.push_back(parameter);
    	}

		int find_parameter_location (std::string parameter)
		{
			for (int i = 0; i < parameter_list.size(); i++) // Search parameter list and return location of matching term
			{
				if(parameter == parameter_list[i]) { return i; }
			}

			return -1; // Return -1 if no matching term is found
    	}

		void clean_parameter_registers () 
		{
        	for (int i = 4; i <= 7; i++) { register_availability_tracker[i] = true; }

			parameter_list.clear();
    	}
		

		/* ------------------------------------					  Frame Pointer Functions					------------------------------------ */
		
		int get_frame_pointer ()
		{
			return frame_pointer_tracker;
		}

		void decrease_frame_pointer () 
		{
			frame_pointer_tracker -= 4;
		}

		void increase_frame_pointer () 
		{
			frame_pointer_tracker += 4;
		}
		
	
		/* ------------------------------------					Global Variable	Functions					------------------------------------ */
		/*
		int find_global_variable (std::string variable) // Return memory location if variable is found
		{
			if (global_variables.find(variable) != global_variables.end())
			{
				return global_variables.find(variable)->second;
			}
			else
			{
				return -1;
			}
		}

		void add_global_variable (std::string variable)
		{
			int memory_location = global_variables.size();
			global_variables.emplace(variable, memory_location);
		}

		void delete_global_variable (std::string variable)
		{
			global_variables.erase(variable);
		}
		*/
		/* ------------------------------------					Local Variable Functions					------------------------------------ */

		int find_local_variable(std::string variable) // Return frame pointer to variable memory location
		{
			// Not dealing with scopes for now

			if (local_variables.find(variable) != global_variables.end())
			{
				return ((local_variables.find(variable)->second) + 1) * 4;
			}
			else
			{
				return -1;
			}
		}

		void add_local_variable(std::string variable)
		{
			int memory_location = local_variables.size();
			local_variables.emplace(variable, memory_location);
		}

		void delete_local_variable(std::string variable)
		{
			local_variables.erase(variable);
		}

		/* ------------------------------------						Scope Functions							------------------------------------ */



};


#endif