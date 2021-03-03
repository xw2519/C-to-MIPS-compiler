#include "../include/ast.hpp"

int main ()
{
	int destination_reg = 2; // v0 register
	
	// Set up AST
	Context context;
	const Node *root = parse();
	
	// Execute compiler
	root->print_MIPS(std::cout, context);

	return 0;
}