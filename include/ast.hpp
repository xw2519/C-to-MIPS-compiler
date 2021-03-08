#ifndef AST
#define AST

#include "ast/ast_declarations.hpp"


extern const Root *parse();
extern void yyset_in(FILE* fd);

#endif
