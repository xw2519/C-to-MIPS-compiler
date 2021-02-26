#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP

#include "ast_statement.hpp"

class Translation_Unit : public Node
{
	protected:
		Node_Ptr left;
		Node_Ptr right;

	private:

	public:
		Translation_Unit(Node_Ptr _left, Node_Ptr _right):left(_left),right(_right){}

		virtual void print_structure(std::ostream &dst) const override{
				left->print_structure(dst);
				dst << "\n";
				right->print_structure(dst);

		}
};

class External_Declaration : public Node{
	virtual void print(std::ostream &dst) const =0;
};

class Function_Definition : public External_Declaration{
	private:
		std::string type;
		std::string id;
		Statement *s_ptr;

	public:
		Function_Definition(std::string _type, std::string _id, Statement *_s_ptr ): type(_type), id(_id), s_ptr(_s_ptr){}

		virtual void print_structure(std::ostream &dst) const override{
			dst << "Type( " << type << " ) " << "Identifier( " << id << " )";
			s_ptr->print_structure(dst);
		}
};

#endif