#ifndef DECLARATIONS
#define DECLARATIONS

#include "ast_node.hpp"
#include "ast_expression.hpp"

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
		PrimitiveType(PrimitiveType *_type1, PrimitiveType *_type2)
		{
			if((_type1.get_type()==INT) || (_type2.get_type()==INT)){
				type = UNSIGNED;
			}else{
				type = UCHAR;
			}
		}

		virtual void print(std::ostream &dst) const override
		{
			switch(type){
				case D_CHAR:     output << "char";          break;
				case D_UCHAR:    output << "unsigned char"; break;
				case D_INT:      output << "int";           break;
				case D_FLOAT:    output << "float";         break;
				case D_DOUBLE:   output << "double";        break;
				case D_UNSIGNED: output << "unsigned";      break;
			}
			output << std::endl;
		}

		virtual DeclarationEnum get_type() const override
		{
			return type;
		}

	  virtual void print_mips(std::ostream &dst, Context& context) const override
		{}
};

class TypeIDType : public FunnyClass
{};

class StructType : public FunnyClass
{};

class EnumType : public FunnyClass
{};

class TypedefDeclaration : public FunnyClass
{};

class Identifier : public FunnyClass
{};

class ArrayDeclarator : public FunnyClass
{};

class FunctionDeclarator : public FunnyClass
{};

class Declarator : public FunnyClass
{};

class FunctionDefinition : public FunnyClass
{};

class Initializer : public FunnyClass
{};

class InitDeclarator : public FunnyClass
{};

class Declaration : public FunnyClass
{};

class Pointer : public FunnyClass
{};


class StructSpecifier : public FunnyClass
{};

class StructDeclaration : public FunnyClass
{};


class EnumSpecifier : public FunnyClass
{};

class Enumerator : public FunnyClass
{};


#endif
