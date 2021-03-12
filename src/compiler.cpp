#include "../include/ast.hpp"

int main (int argc, char* argv[])
{
	yyin = fopen(argv[2], "r");
	std::ofstream output_file(argv[4]);

	// Set up AST
	Context context;
	const Node* root = parse();
	
	// Execute compiler
	root->compile(output_file, context);

	return 0;
}