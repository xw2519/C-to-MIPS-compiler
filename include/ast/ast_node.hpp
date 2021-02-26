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


typedef const Root* RootPtr; // Pointer to Node

class Root
{
protected:
    std::string type_;

public:
    virtual ~Root() {}

    virtual const std::string &getType() const
    {
        return type_;
    };
};

#endif