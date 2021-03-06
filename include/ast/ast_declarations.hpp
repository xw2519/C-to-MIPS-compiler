#ifndef DECLARATIONS
#define DECLARATIONS

#include "ast_node.hpp"
#include "ast_expressions.hpp"

enum DeclarationEnum { D_CHAR, D_UCHAR, D_INT, D_FLOAT, D_DOUBLE, D_UNSIGNED };

class FunnyClass : public Node
{
  protected:
    DeclarationEnum type;
	public:
		virtual void print(std::ostream &dst) const =0;
		virtual DeclarationEnum get_type() const =0;
		virtual void print_mips(std::ostream &dst, Context& context) const =0;
};

class PrimitiveType : public FunnyClass
{
  public:
    PrimitiveType(DeclarationEnum _type): type(_type) {}

		virtual void print(std::ostream &dst) const override
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

		virtual DeclarationEnum get_type() const override
		{
			return type;
		}

	  virtual void print_mips(std::ostream &dst, Context& context) const override
		{}
};

class Identifier : public FunnyClass
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
};

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
};

class TypeIDType : public FunnyClass
{
	protected:
		std::string type_id;
	public:
		TypeIDType(std::string &_type_id)
		: type_id(_type_id) {}
};

class StructType : public FunnyClass
{
	protected:
		StructSpecifier* declr;
	public:
		StructType(StructSpecifier* _declr)
		: declr(_declr) {}
};

class EnumType : public FunnyClass
{
	protected:
		EnumSpecifier* declr;
	public:
		EnumType(EnumSpecifier* _declr)
		: declr(_declr) {}
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
		FunnyClass* declr;
		Expression* size;
	public:
		ArrayDeclarator(FunnyClass* _declr, Expression* _size)
		: declr(_declr), size(_size) {}
};

class FunctionDeclarator : public FunnyClass
{
	protected:
		FunnyClass* declr;
	public:
		FunctionDeclarator(FunnyClass* _declr)
		: declr(_declr) {}
};

class Declarator : public FunnyClass
{
	protected:
		FunnyClass* declr;
		int point_depth;
	public:
		Declarator(FunnyClass* _declr, Pointer* pointer=NULL) : declr(_declr)
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
};

class FunctionDefinition : public FunnyClass
{
	protected:
		std::vector<PrimitiveType*>* return_type;
		Declarator* declr;
		CompoundStatement* statement;
	public:
		FunctionDefinition(std::vector<PrimitiveType*>* _return_type, Declarator* _declr, CompoundStatement* _statement)
		: return_type(_return_type), declr(_declr), statement(_statement) {}
};


class StructSpecifier : public FunnyClass
{
	protected:
		std::string ident;
		std::vector<StructDeclaration*>* member_declrs;
	public:
		StructSpecifier(std::string &_ident, std::vector<StructDeclaration*>* _member_declrs==NULL)
		: ident(_ident), member_declrs(_member_declrs) {}
		StructSpecifier(std::vector<StructDeclaration*>* _member_declrs)
		: member_declrs(_member_declrs) {}
};

class StructDeclaration : public FunnyClass
{
	protected:
		PrimitiveType* type;
		std::vector<Declarator*>* declrators;
	public:
		StructDeclaration(PrimitiveType* _type, std::vector<Declarator*>* _declarators)
		: type(_type), declarators(_declarators) {}
};


class EnumSpecifier : public FunnyClass
{
	protected:
		std::string ident;
		std::vector<Enumerator*>* enum_declrs;
	public:
		StructSpecifier(std::string &_ident, std::vector<Enumerator*>* _enum_declrs==NULL)
		: ident(_ident), enum_declrs(_enum_declrs) {}
		StructSpecifier(std::vector<Enumerator*>* _enum_declrs)
		: enum_declrs(_enum_declrs) {}
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


#endif
