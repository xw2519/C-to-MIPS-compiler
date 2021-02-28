#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP

#include "ast_expression.hpp"

#include <iomanip>

class Statement : public Node {};

class Translation_Unit : public Node
{
	protected:
		Node_Ptr left;
		Node_Ptr right;

	private:

	public:
		Translation_Unit(Node_Ptr _left, Node_Ptr _right) : left(_left), right(_right) {}
};

class External_Declaration : public Node {};

class Declarator : public External_Declaration
{
	private:
		std::string ID;
		Expression *initialisation_expression;

	public:
		std::string getID()
		{
			return ID;
		}

		Declarator(std::string _ID = "", Expression *initialisation_expression = NULL) 
		: ID(_ID), initialisation_expression(initialisation_expression) {}
};


class Declaration : public External_Declaration
{
	private:
		std::string type;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: type(_TYPE), declaration_list(_declaration_list) {}
};

class Function_Definition : public External_Declaration
{
	private:
		std::string TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement*	statement_ptr;

	public:
		Function_Definition(std::string _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement *_statement_ptr ) 
		: TYPE(_TYPE), ID(_ID), parameter_list(_parameter_list), statement_ptr(_statement_ptr) {}
};


#endif