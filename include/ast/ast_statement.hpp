#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP

#include "ast_expression.hpp"
#include "ast_declaration.hpp"

#include <iomanip>

class Expression_Statement : public Statement
{
	private:
		Expression *expression;

	public:
		Expression_Statement( Expression* _expression ) : expression(_expression) {}

		virtual void print_structure(std::ostream &dst, int m) const override{
			dst << std::setw(m) << "";
			dst << "ExpressionStatement [ ";
			expression->print_structure(dst,m);
			dst << " ]" << std::endl;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override{
			expression->print_C(dst,indent);
			dst << ";";
		}
};

class Compound_Statement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		Compound_Statement(std::vector<Declaration*>* _declaration_list = NULL, std::vector<Statement*>* _statement_list = NULL)
		:statement_list(_statement_list), declaration_list(_declaration_list) {}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst<<indent<<"{"<<std::endl;
			if(declaration_list != NULL)
			{
				for(auto it=declaration_list->begin();it!=declaration_list->end();it++)
				{
					(*it)->print_C(dst,indent+"    ");
					dst << ";" << std::endl;
				}
			}

			if(statement_list !=NULL)
			{		
				for(auto it=statement_list->begin();it!=statement_list->end();it++)
				{
					(*it)->print_C(dst,indent+"    ");
					dst << std::endl;
				}
			}
			dst<<indent<<"}"<<std::endl;
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst <<  std::setw(m) << "";
			dst << "Compound_Statement [" << std::endl;

			if(declaration_list != NULL)
			{		
				for(auto it=declaration_list->begin();it!=declaration_list->end();it++)
				{
					(*it)->print_structure(dst,m+2);
				}
			}

			if(statement_list !=NULL)
			{		
				for(auto it=statement_list->begin();it!=statement_list->end();it++)
				{
					(*it)->print_structure(dst,m+2);
				}
			}

			dst << std::setw(m) << "";
			dst << "]" << std::endl;
		}
};

class Condition_If_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		Condition_If_Statement(Expression* _condition_expression, Statement* _true_statement)
		:condition_expression(_condition_expression),true_statement(_true_statement) {}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			dst << indent << "if (";
			condition_expression->print_C(dst,"");
			dst << ")" << std::endl;
			true_statement->print_C(dst,indent);
			dst << std::endl;
		}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << std::setw(m) << "";
			dst << "If Statement [ " ;
			condition_expression->print_structure(dst,m+2);
			dst << std::endl;
			true_statement->print_structure(dst,m+2);
			dst << std::setw(m) << "";
			dst << " ]" << std::endl;
		}
};

class Condition_If_Else_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* 	true_statement;
		Statement* 	false_statement;
		
	public:
		Condition_If_Else_Statement(Expression* _condition_expression, Statement* _true_statement, Statement* _false_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement), false_statement(_false_statement) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << std::setw(m) << "";
			dst << "If Else Statement [ " ;
			condition_expression->print_structure(dst,m+2);
			dst << std::endl;

			dst << std::setw(m+2) << "";
			dst << "Condition True [ " << std::endl;
			true_statement->print_structure(dst,m+4);
			dst << std::setw(m+2) << "";
			dst << "]" << std::endl;

			dst << std::setw(m+2) << "";
			dst << "Condition False [ " << std::endl;
			false_statement->print_structure(dst,m+4);
			dst << std::setw(m+2) << "";
			dst << "]" << std::endl;

			dst << std::setw(m) << "";
			dst << "]" << std::endl;
		}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			dst << indent << "if(";
			condition_expression->print_C(dst,"");
			dst << ")" << std::endl;
			true_statement->print_C(dst, indent);
			dst << std::endl;
			dst << indent << "else" << std::endl;
			false_statement->print_C(dst, indent);
			dst << std::endl;
		}
};

class While_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		While_Statement(Expression* _condition_expression, Statement* _true_statement)
		:condition_expression(_condition_expression),true_statement(_true_statement) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << std::setw(m) << "";
			dst << "While_Statement [ " ;
			condition_expression->print_structure(dst,m+2);
			dst << std::endl;
			true_statement->print_structure(dst,m+2);
			dst << std::setw(m) << "";
			dst << " ]" << std::endl;
		}

		virtual void print_C(std::ostream &dst, std::string indent) const override
		{
			dst << indent << "while (";
			condition_expression->print_C(dst,"");
			dst << ")" << std::endl;
			true_statement->print_C(dst,indent);
		}
};

class For_Statement : public Statement
{
	private:
		Expression* initialisation_expression;
		Expression* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;
		
	public:
		For_Statement(Expression* _init_expr, Expression* _condition_expression, Expression* _update_expression, Statement* _true_statement)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << "ForStatement" << std::endl;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << indent << "for(";
			initialisation_expression->print_C(dst,"");
			dst << ";";
			condition_expression->print_C(dst,"");
			dst << ";";
			update_expression->print_C(dst,"");
			dst << ")" << std::endl;
			true_statement->print_C(dst,indent);
		} 
};

class Jump_Statement : public Statement
{
	private:
		Expression* expression;

	public:
		Jump_Statement( Expression* _expression = NULL)
		:expression(_expression) {}

		virtual void print_structure(std::ostream &dst, int m) const override
		{
			dst << std::setw(m) << "";
			dst << "JumpStatement [ ";
			if(expression!=NULL){
				expression->print_structure(dst,m);
			}
			dst << "; ]" << std::endl;
		}

		virtual void print_C(std::ostream &dst,std::string indent) const override
		{
			dst << indent << "return";
			if(expression!=NULL) expression->print_C(dst," ");
			dst << ";";
		}
};

#endif