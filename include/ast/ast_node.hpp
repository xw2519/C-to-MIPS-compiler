#ifndef NODE
#define NODE

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <regex>
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <initializer_list>

struct Context;


class Node
{
	public:


		virtual ~Node()
		{}

		virtual void print_mips (std::ostream &dst, Context& context) const
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};


#endif
