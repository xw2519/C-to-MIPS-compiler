#ifndef AST
#define AST

// Include headers in ast that contains the classes for the tree nodes.
#include "ast/ast_node.hpp"
#include "ast/ast_expressions.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_declarations.hpp"
#include "ast/ast_context.hpp"

extern const Root *parse();
extern void yyset_in(FILE* fd);

#endif
