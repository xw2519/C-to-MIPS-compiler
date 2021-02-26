#ifndef ast_hpp
#define ast_hpp

/* ---------------------- Include all headers in AST ---------------------- */
#include "ast/ast_node.hpp"
#include "ast/ast_declaration.hpp"
#include "ast/ast_expression.hpp"
#include "ast/ast_operators.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_statement.hpp"

#include <vector>

extern const Node* parse();
extern void yyset_in(FILE* fd);

#endif