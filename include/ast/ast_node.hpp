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

class Node;

typedef const Node* Node_Ptr;

class Node
{
	public:
		virtual ~Node () {}
		virtual void print_structure 	(std::ostream &dst) const =0;
		virtual void print_C 			(std::ostream &dst, std::string indent) const =0;
		virtual void print_mips 		(std::ostream &dst) const =0;

};

#endif