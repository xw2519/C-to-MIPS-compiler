#ifndef AST_NODE_HPP
#define AST_NODE_HPP

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

#include "ast_context.hpp"

class Node;

typedef const Node* Node_Ptr;

class Node
{
	public:
		virtual ~Node () {}

		virtual void print_structure 	(std::ostream &dst, int m) const =0;

		virtual void print_C 			(std::ostream &dst, std::string indent) const
		{
			std::cerr << "Node.hpp: 'print_C' not implemented on " << typeid(this).name() << "\n";
		}
		
		virtual void print_MIPS 		(std::ostream &dst, Context& ctx) const 
		{
			std::cerr<<"ASTNode::MIPS compilation is not implemented by type "<<typeid(this).name()<<"\n";
		}
};

#endif