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

$8 	- $15				Temporary registers that can be overwritten by called procedures
$16 - $23				Temporary registers but cannot be overwritten by procedures	
$24	- $25				Temporary registers 

*/

static int global_variable_counter;
static int local_variable_counter;

struct Context
{
	private:
		std::vector <std::string> parameter_list;

		// Mapping of global and local bindings with binding name as the key
		std::map <std::string, int> global_binding;
		std::map <std::string, int> local_binding;

		// Mapping of variable to location in memory with variable name as the key
		std::map <std::string, int> variable_location;

		// Register availability tracker
		bool register_availability_tracker[32]; // Register $0 to $31
	
	public:
		~Context () {}

		Context () 
		{
			// Initialise the register availability tracker
			for (int i = 0; i < 32; i++) 
			{
				if (i <= 3 || i >= 26) // TRUE: Available and FALSE: Unavailable
				{
					register_availability_tracker[i] = false;
				}
				else
				{
					register_availability_tracker[i] = true;
				}
        	}

			// Set up variable counters
			global_variable_counter = 0;
			local_variable_counter = 0;
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
        	// std::cerr << "Set available: " << register_ID << std::endl;
        	register_availability_tracker[register_ID] = true;
    	}

		void set_unavaiable (int register_ID) 
		{
        	// std::cerr<< "Set unavailable: " << register_ID << std::endl;
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
			std::vector<int> available_constant_registers;

			for (int i = 16; i <= 23; i++) 
			{
				if (register_availability_tracker[i] == false) 
				{
					available_constant_registers.push_back(i);
				}
        	}    

        	return available_constant_registers;
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

		/* ------------------------------------					Variable Register Functions					------------------------------------ */

		int find_variable_location (std::string variable) 
		{
			if (variable_location.find(variable) != variable_location.end())
			{
				return (variable_location.find(variable)->second);
			}
			else
			{
				return -1; // Return -1 if no matching term is found
			}
    	}

		void store_variable (int location, std::string variable) 
		{
			// Remove the old variable location
        	variable_location.erase(variable); 

			// Place new variable and location into map
			variable_location.emplace(variable, location); 
    	}

		void delete_variable(std::string variable) 
		{ 
			variable_location.erase(variable); 
		}

		/* ------------------------------------					Local Register Functions					------------------------------------ */

		void store_local_variable (std::string variable) 
		{
			++local_variable_counter;
        	local_binding.emplace(variable, local_variable_counter);
    	}

		void delete_local_variable (std::string variable) 
		{
        	local_binding.erase(variable);
    	}

		/* ------------------------------------					Global Register Functions					------------------------------------ */

		void store_global_variable (std::string variable) 
		{	
			++global_variable_counter;
        	global_binding.emplace(variable, global_variable_counter);
    	}

		void delete_global_variable (std::string variable) 
		{
        	global_binding.erase(variable);
    	}

};


#endif