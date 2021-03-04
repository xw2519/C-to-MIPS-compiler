#ifndef ast_hpp
#define ast_hpp

/* ----------------------       Include all headers in AST          ---------------------- */
#include "ast/ast_node.hpp"
#include "ast/ast_context.hpp"
#include "ast/ast_declaration.hpp"
#include "ast/ast_expression.hpp"
#include "ast/ast_primitive.hpp"
#include "ast/ast_statement.hpp"
#include "ast/ast_variable.hpp"

/* ----------------------          Include all libraries            ---------------------- */
#include "stdio.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <typeinfo>

extern const Node* parse();
extern FILE* yyin;		 

#endif