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


class Node
{
	public:
		virtual ~Node () {}

		virtual void print_MIPS (std::ostream &dst, Context& context) const
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};

#endif
