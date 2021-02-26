#ifndef ast_hpp
#define ast_hpp

// Include headers in ast that contains the classes for the tree nodes.
#include "ast/ast_node.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_expressions.hpp"
#include "ast/ast_functions.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_enums.hpp"

#include <vector>

extern std::vector<const Node*> parseAST();
extern void yyset_in(FILE* fd);

#endif