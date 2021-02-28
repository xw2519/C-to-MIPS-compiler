#include "../include/ast.hpp"

int main()
{

	const Node *root;

	Context context;

	root = new Add_Expression(new Constant(5), new Constant(11));

	root->print_C(std::cout, " ");

	return 0;
}