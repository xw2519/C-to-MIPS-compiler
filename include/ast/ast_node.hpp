#ifndef ast_node_hpp
#define ast_node_hpp

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


class Root
{
protected:
    std::string type_;

public:
    virtual ~Root() {}

    virtual const std::string &getType() const
    {
        return type_;
    }

    virtual void print_mips (std::ostream &dst, Context& ctx, int destination_register) const
		{
			std::cerr << "Ast_node.hpp: 'print_MIPS' not implemented on " << typeid(this).name() << "\n";
		}

}

#endif
