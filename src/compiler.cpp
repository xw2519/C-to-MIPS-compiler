#include "../include/ast.hpp"

int main(){

	const Node *root = parse();
	root->print_mips(std::cout);
	std::cout << std::endl;

	return 0;
}