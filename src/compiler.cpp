#include "../include/ast.hpp"

int main()
{

	const Node *root;

	Context c;
	root = new Add_Expression(new Constant(5), new Constant(11));
	root->print_MIPS(std::cout,c);

	return 0;
}