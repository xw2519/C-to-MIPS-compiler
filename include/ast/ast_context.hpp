#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

/*
$0	        $zero	    Hard-wired to 0
$1	        $at	        Reserved for pseudo-instructions
$2  - $3	$v0,  $v1	Return values from functions
$4  - $7	$a0 - $a3	Arguments to functions - not preserved by subprograms
$8  - $15	$t0 - $t7	Temporary registers, not preserved by subprograms
$16 - $23	$s0 - $s7	Saved registers, preserved by subprograms
$24 - $25	$t8 - $t9	More temporary registers, not preserved by subprograms
$26 - $27	$k0 - $k1	Reserved for kernel. Do not use.
$28	        $gp	        Global Area Pointer (base of global data segment)
$29	        $sp	        Stack Pointer
$30	        $fp	        Frame Pointer
$31	        $ra	        Return Address
*/

class Node;


typedef const Node* Node_Ptr;

struct TranslateContext{
	std::vector<std::string> global_var;
};


struct Context{
	std::vector< std::pair<std::string,std::string> > var_bindings_stack; //global,local : <var_name,memloc>, 
																		  //most recent represents the one visible 
																		  //to the scope
	int temp_reg_free = 0; 	//t0-t9
	int saved_reg_free = 0;	//s0-s7
	std::string curr_dest_reg = "t0";




	void reset_free_reg(){
		temp_reg_free = 0;
		saved_reg_free = 0;
		curr_dest_reg = "t0";
	}

	std::string get_dest_reg(){
		return curr_dest_reg;
	}

	std::string alloc_free_reg(){
		if(temp_reg_free <= 9){
			temp_reg_free++;
			curr_dest_reg = "t"+std::to_string(temp_reg_free);
			return curr_dest_reg;
		}
		else if(saved_reg_free <= 7){
			curr_dest_reg = "s"+std::to_string(saved_reg_free);
			saved_reg_free++;
			return curr_dest_reg;
		}
		else{
			std::cout << "Out of registers" << std::endl;
			return "NULL";
		}
	}
};


#endif