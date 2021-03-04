#ifndef AST_NODE_HPP
#define AST_NODE_HPP

/* ------------------------------------ 				   			   Libraries 								------------------------------------ */

#include <string>
#include <iostream>
#include <cmath>
#include <regex>
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <initializer_list>


/* ------------------------------------ 			  			   Custom header files							------------------------------------ */

#include "ast_context.hpp"

/* ------------------------------------ 			  			    Custom definitions		 					------------------------------------ */

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

/* ------------------------------------ 			      			   Node class		 			 			------------------------------------ */
class Node;

typedef const Node* Node_Ptr;

class Node
{
	public:
		virtual ~Node () {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const 
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};

/* ------------------------------------ 			      			  Variable class		 					------------------------------------ */
 // Handles the properties of the variable including:
 // 	- Context scope
 //		- Variable type
 //		- Declaration 
 //		- Memory address

class variable
{
	private: 
		int variable_address;
		context_scope variable_scope;
		declaration_type variable_declaration;
		type variable_type;

	public:
		variable (int _variable_address, context_scope _variable_scope, declaration_type _variable_declaration, type _variable_type) 
		: variable_address(variable_address), variable_scope(_variable_scope), variable_declaration(_variable_declaration), variable_type(_variable_type) {}

		// Get attributes
		int get_variable_address() { return variable_address; }
		context_scope get_variable_scope() { return variable_scope; }
		type get_variable_type() { return variable_type; }
};

/* ------------------------------------ 			      			    Type class		 						------------------------------------ */
 // Handles the properties of the variable types including:
 // 	- The type of variable e.g. int, void
 //		- Parameters or values attached with the variable type e.g. arrays
 //		- Array tracker variables 

class type_definition
{
	private: 
		// Variable type set to NONE by default
		type variable_type = NONE;

		// Array trackers (default set to 0)
		int array_access_tracker = 0;
		int array_size_tracker = 0;

	public:
		type_definition(type _variable_type) : variable_type(_variable_type) {}

		// Variable type oeprators
		type get_variable_type() { return variable_type; }
};


#endif