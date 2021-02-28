#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP

#include "ast_expression.hpp"

#include <iomanip>

class Statement : public Node 
{

};

class Translation_Unit : public Node
{
	protected:
		Node_Ptr left;
		Node_Ptr right;

	private:

	public:
		Translation_Unit(Node_Ptr _left, Node_Ptr _right):left(_left),right(_right){}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			left->print_structure(dst,m);
			dst << std::endl;
			right->print_structure(dst,m);
		}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			left->print_C(dst, indent);
			dst << ";";
			dst << std::endl;
			right->print_C(dst, indent);
			dst << ";";
		}
};

class External_Declaration : public Node
{
	virtual void print_structure(std::ostream &dst, int m) const =0;
};

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

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst <<  std::setw(m) << "";
			dst << "Declarator [ ";
			dst << "Id ( " << ID << " ) ";
			if(initialisation_expression != NULL){
				dst << ", initialisation_expression ( ";
				initialisation_expression->print_structure(dst, m);
				dst << " )";
			}
			dst << " ]" << std::endl;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << indent << ID;
			if(initialisation_expression != NULL){
				dst << " =";
				initialisation_expression->print_C(dst,"");
			}
		}
};


class Declaration : public External_Declaration
{
	private:
		std::string type;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(std::string _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: type(_TYPE), declaration_list(_declaration_list)
		{

		}

		virtual void print_structure(std::ostream &dst, int m) const override{
			dst <<  std::setw(m) << "";
			dst << "Declaration [" << std::endl;
			dst <<  std::setw(m+2) << "";
			dst << "Type( " << type << " ), " << std::endl;
			dst <<  std::setw(m) << "";
			dst << "]" << std::endl;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override{
			dst << indent << type << " ";
			if(declaration_list != NULL){
				for(auto it = declaration_list->begin(); it != declaration_list->end(); it++){
					(*it)->print_C(dst,"");
					if(next(it,1) != declaration_list->end()) dst << ",";
				}
			}
		}	
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

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			dst << TYPE << " " << ID << "(";
			if(parameter_list != NULL){
				for(auto it = parameter_list->begin(); it != parameter_list->end(); it++)
				{
					(*it)->print_C(dst,"");
					if(next(it,1) != parameter_list->end()) dst << ", ";
				}
			}
			dst <<")";
			if(statement_ptr != NULL)
			{
				statement_ptr->print_C(dst,indent);
			}
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << "FunctionDefinition [ " << std::endl;
			dst <<  std::setw(m+2) << "";
			dst << "Type( " << TYPE << " )"<<  std::endl;
			dst <<  std::setw(m+2) << "";
			dst << "Identifier( " << ID << " )" << std::endl;

			if(parameter_list != NULL)
			{
				dst <<  std::setw(m+2) << "";
				dst << "Parameter List (" << std::endl;
			}

			dst <<  std::setw(m+2) << "";
			dst << ")" << std::endl;
			statement_ptr->print_structure(dst,m+2);
			dst << "]" << std::endl;
		}
};


#endif