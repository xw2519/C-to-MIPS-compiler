#include "../include/ast.hpp"

int main()
{

	const Node *root = parse();

	Context context;

	root->print_MIPS(std::cout, context);

	return 0;
}