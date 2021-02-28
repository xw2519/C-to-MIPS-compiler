#ifndef AST_NODE_HPP
#define AST_NODE_HPP

/* -------------------------------- 				Libraries 				-------------------------------- */

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


/* -------------------------------- 			Custom header files 		-------------------------------- */

#include "ast_context.hpp"


/* -------------------------------- 			  Static variables		 	-------------------------------- */

static int scope;
static int variable_counter;

static std::vector<std::string> global_variables; 


/* -------------------------------- 			     Node class		 		-------------------------------- */
class Node;

typedef const Node* Node_Ptr;

class Node
{
	public:
		virtual ~Node () {}

		virtual void print_MIPS (std::ostream &dst, Context& ctx, int destination_register) const 
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};

#endif