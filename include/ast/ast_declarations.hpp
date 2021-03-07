#ifndef DECLARATIONS
#define DECLARATIONS

#include "ast_node.hpp"
#include "ast_expressions.hpp"

struct Context;
class Statement;
class PrimitiveType;

enum DeclarationEnum { D_CHAR, D_UCHAR, D_INT, D_FLOAT, D_DOUBLE, D_UNSIGNED, TYPEDEF, TYPEID };

class FunnyClass: public Node{
  protected:
    DeclarationEnum type;
	/*public:
		virtual void print(std::ostream &dst) const =0;
		virtual void print_mips(std::ostream &dst, Context& context) const =0;*/
};


/*class Identifier : public FunnyClass
{
	protected:
		std::string ident;
	public:
		Identifier(std::string &_ident)
		: ident(_ident) {}

		virtual void print(std::ostream &dst) const override
		{
			dst << ident << std::endl;
		}

		virtual DeclarationEnum get_type() const override
		{
			return IDENTIFIER;
		}

		virtual std::string get_value() const
		{
			return ident;
		}

		virtual void print_mips(std::ostream &dst, Context& context) const override
		{}
};*/

class Pointer : public FunnyClass
{
	protected:
		int depth;
	public:
		Pointer()
		{
			depth = 1;
		}
		Pointer(Pointer* prec)
		{
			depth = prec->get_depth()+1;
			delete prec;
		}

    int get_depth()
    {
      return depth;
    }
};

class TypeIDType : public FunnyClass
{
	protected:
		std::string type_id;
	public:
		TypeIDType(std::string &_type_id)
		: type_id(_type_id) {}
};

class TypedefDeclaration : public FunnyClass
{
	protected:
		std::vector<PrimitiveType*>* types;
	public:
		TypedefDeclaration(std::vector<PrimitiveType*>* _types)
		: types(_types) {}
};


class ArrayDeclarator : public FunnyClass
{
	protected:
		Node* declr;
		Expression* size;
	public:
		ArrayDeclarator(Node* _declr, Expression* _size=NULL)
		: declr(_declr), size(_size) {}
};

class FunctionDeclarator : public FunnyClass
{
	protected:
		Node* declr;
	public:
		FunctionDeclarator(Node* _declr)
		: declr(_declr) {}
};

class Declarator : public FunnyClass
{
	protected:
		Node* declr;
		int point_depth;
	public:
		Declarator(Node* _declr, Pointer* pointer=NULL) : declr(_declr)
		{
			if(pointer){
				point_depth = pointer->get_depth();
				delete pointer;
			}
		}
};

class Initializer : public FunnyClass
{
	protected:
		std::vector<Initializer*>* init_list;
		Expression* expr;
	public:
		Initializer(std::vector<Initializer*>* _init_list=NULL, Expression* _expr=NULL)
		: init_list(_init_list), expr(_expr) {}
    Initializer(Expression* _expr)
		: expr(_expr) {}
};

class InitDeclarator : public FunnyClass
{
	protected:
		Declarator* declr;
		Initializer* init;
	public:
		InitDeclarator(Declarator* _declr, Initializer* _init=NULL)
		: declr(_declr), init(_init) {}
};

class Declaration : public FunnyClass
{
	protected:
		std::vector<PrimitiveType*>* types;
		std::vector<InitDeclarator*>* declarators;
	public:
		Declaration(std::vector<PrimitiveType*>* _types, std::vector<InitDeclarator*>* _declarators=NULL)
		: types(_types), declarators(_declarators) {}
    Declaration(Node* _typedef, std::vector<Node*>* _nodes) {}  // should never happen
};

class FunctionDefinition : public FunnyClass
{
	protected:
		std::vector<PrimitiveType*>* return_type;
		Declarator* declr;
		Statement* statement;
	public:
		FunctionDefinition(std::vector<PrimitiveType*>* _return_type, Declarator* _declr, Statement* _statement)
		: return_type(_return_type), declr(_declr), statement(_statement) {}
};


class StructDeclaration : public FunnyClass
{
	protected:
		PrimitiveType* type;
		std::vector<Declarator*>* declarators;
	public:
		StructDeclaration(PrimitiveType* _type, std::vector<Declarator*>* _declarators)
		: type(_type), declarators(_declarators) {}
};

class StructSpecifier : public FunnyClass
{
	protected:
		std::string ident;
		std::vector<StructDeclaration*>* member_declrs;
	public:
		StructSpecifier(std::string &_ident, std::vector<StructDeclaration*>* _member_declrs=NULL)
		: ident(_ident), member_declrs(_member_declrs) {}
		StructSpecifier(std::vector<StructDeclaration*>* _member_declrs)
		: member_declrs(_member_declrs) {}
};


class Enumerator : public FunnyClass
{
	protected:
		std::string ident;
		Expression* expr;
	public:
		Enumerator(std::string &_ident, Expression* _expr=NULL)
		: ident(_ident), expr(_expr) {}
};

class EnumSpecifier : public FunnyClass
{
	protected:
		std::string ident;
		std::vector<Enumerator*>* enum_declrs;
	public:
		EnumSpecifier(std::string &_ident, std::vector<Enumerator*>* _enum_declrs=NULL)
		: ident(_ident), enum_declrs(_enum_declrs) {}
		EnumSpecifier(std::vector<Enumerator*>* _enum_declrs)
		: enum_declrs(_enum_declrs) {}
};


class PrimitiveType : public FunnyClass
{
  protected:
    DeclarationEnum type;
    std::string name;
    StructSpecifier* struct_declr;
    EnumSpecifier* enum_declr;

  public:
    PrimitiveType(DeclarationEnum _type): type(_type) {}
    PrimitiveType(DeclarationEnum _type, std::string &_name): type(_type), name(_name) {}
    PrimitiveType(StructSpecifier* _struct_declr): struct_declr(_struct_declr) {}
    PrimitiveType(EnumSpecifier* _enum_declr): enum_declr(_enum_declr) {}
    PrimitiveType(PrimitiveType* type1, PrimitiveType* type2)
    {
      if((type1->get_type() == INT) || (type2->get_type() == INT)){
        type = D_UNSIGNED;
      }else{
        type = D_UCHAR;
      }
      delete type1;
      delete type2;
    }

		virtual void print(std::ostream &dst) const
		{
			switch(type){
				case D_CHAR:     dst << "char";          break;
				case D_UCHAR:    dst << "unsigned char"; break;
				case D_INT:      dst << "int";           break;
				case D_FLOAT:    dst << "float";         break;
				case D_DOUBLE:   dst << "double";        break;
				case D_UNSIGNED: dst << "unsigned";      break;
			}
			dst << std::endl;
		}

		virtual DeclarationEnum get_type() const
		{
			return type;
		}

	  virtual void print_mips(std::ostream &dst, Context& context) const
		{}
};


class Root : public Node
{
	protected:
		std::vector<Node*>* translational_unit;
	public:
		Root(std::vector<Node*>* _translational_unit)
		: translational_unit(_translational_unit) {}
};

#endif
