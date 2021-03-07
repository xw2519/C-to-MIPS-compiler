#include "../include/ast.hpp"


int main ()
{
	// Set up AST
	Context context;
	const Root *root = parse();

	// Execute compiler
	root->print_mips(std::cout, context);

	return 0;
}
