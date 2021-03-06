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
	private:
		std::vector<FunnyClass*>* translational_unit;
	public:
		Node(std::vector<FunnyClass*>* _translational_unit)
		: translational_unit(_translational_unit) {}

		virtual ~Node()
		{
			for(int i=0; i<translational_unit->size(); i++){
				delete translational_unit->at(i);
			}
			delete translational_unit;
		}

		virtual void print_mips (std::ostream &dst, Context& context) const
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}
};

#endif
