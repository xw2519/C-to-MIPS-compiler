#ifndef ast_expression_hpp
#define ast_expression_hpp

#include "ast_node.hpp"

#include <string>
#include <iostream>
#include <map>
#include <memory>

class Expression;

typedef const Expression *Expression_Ptr;

class Expression : public Node
{
public:
    virtual ~Expression()
    {}

    // Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst) const =0;

    // Evaluate tree using the given mapping of variables to numbers
    virtual int evaluate( const std::map<std::string,double> &bindings) const
    { 
		throw std::runtime_error("Expression 'evaluate' not implemented."); 
	}
};

#endif