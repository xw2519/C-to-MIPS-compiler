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

/* ------------------------------------ 			      			   Node class		 			 			------------------------------------ */
class Node;

typedef const Node* Node_Ptr;

class Node
{
	public:
		virtual ~Node () {}

		virtual void compile(std::ostream& dst, Context& context) const 
		{
			std::cerr << "Ast_node.hpp: 'compile' not implemented on " << typeid(this).name() << "\n";
		}
};



#endif