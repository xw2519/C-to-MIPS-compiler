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


typedef const Node* Node_Ptr; // Pointer to Node

class Node
{
protected:
    std::string type_;

public:
    virtual ~Node() {}

    virtual const std::string &getType() const
    {
        return type_;
    };
};

#endif