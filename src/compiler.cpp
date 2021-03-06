#include "../include/ast.hpp"


int main ()
{
	// Set up AST
	Context context;
	const Node *root = parse();

	// Execute compiler
	root->compile(std::cout, context);

	return 0;
}
