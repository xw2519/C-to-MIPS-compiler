#ifndef DECLARATIONS
#define DECLARATIONS
#include "ast_statements.hpp"


class FunnyClass: public Node                                  // complete
{
  protected:
    DeclarationEnum type;
	public:
    virtual std::string get_id() const =0;
		virtual void print(std::ostream &dst) const =0;
		virtual void print_mips(std::ostream &dst, Context& context) const =0;
};

class DeclarationIdentifier : public FunnyClass
{
protected:
  std::string value;

public:
  DeclarationIdentifier(std::string &_ID)
  : value(_ID) {}

  virtual std::string get_id() const override
  {
    return value;
  }

  virtual void print(std::ostream &dst) const override
  {
    dst << value;
  }

  virtual void print_mips(std::ostream &dst, Context &context) const
  {
    context.add_identifier(value);
  }
};

class Pointer : public FunnyClass                              // complete
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

    virtual int get_depth() const
    {
      return depth;
    }

    virtual std::string get_id() const override
    {
      return "pointer";
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << "pointer " << depth;
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {}
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

    virtual std::string get_id() const
    {
      return declr->get_id();
    }

    virtual void print(std::ostream &dst) const override {}
		virtual void print_mips(std::ostream &dst, Context& context) const override {}
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

    ~StructSpecifier()
    {
      for(int i=0; i<member_declrs->size(); i++){
        delete member_declrs->at(i);
      }
      delete member_declrs;
    }

    virtual std::string get_id() const
    {
      return ident;
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << "struct " << ident;
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      context.add_struct(ident, member_declrs);
    }
};

class Enumerator : public FunnyClass
{
	protected:
		std::string ident;
		Expression* expr;
	public:
		Enumerator(std::string &_ident, Expression* _expr=NULL)
		: ident(_ident), expr(_expr) {}

    ~Enumerator()
    {
      delete expr;
    }

    virtual std::string get_id() const
    {
      return ident;
    }

    virtual int get_value() const
    {
      return expr->evaluate();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << ident << expr->evaluate();
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {}
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

    ~EnumSpecifier()
    {
      for(int i=0; i<enum_declrs->size(); i++){
        delete enum_declrs->at(i);
      }
      delete enum_declrs;
    }

    virtual std::string get_id() const
    {
      return ident;
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << "enum " << ident;
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      context.add_enum(ident, enum_declrs);
    }
};

class PrimitiveType : public FunnyClass                        // complete
{
  protected:
    DeclarationEnum type;
    std::string name;
    StructSpecifier* struct_declr;
    EnumSpecifier* enum_declr;

  public:
    PrimitiveType(DeclarationEnum _type)
    : type(_type) {}
    PrimitiveType(DeclarationEnum _type, std::string &_name)
    : type(_type), name(_name) {}
    PrimitiveType(DeclarationEnum _type, StructSpecifier* _struct_declr)
    : type(_type), struct_declr(_struct_declr) {}
    PrimitiveType(DeclarationEnum _type, EnumSpecifier* _enum_declr)
    : type(_type), enum_declr(_enum_declr) {}
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

    ~PrimitiveType()
    {
      if(struct_declr){ delete struct_declr; }
      if(enum_declr){ delete enum_declr; }
    }

    virtual DeclarationEnum get_type() const
		{
			return type;
		}

    virtual std::string get_id() const override
    {
      if(type==TYPEID){
        return name;
      }else if(type==STRUCTS){
        return struct_declr->get_id();
      }else if(type==ENUMS){
        return enum_declr->get_id();
      }else if(type==D_CHAR){
        return "char";
      }else if(type==D_UCHAR){
        return "uchar";
      }else if(type==D_INT){
        return "int";
      }else if(type==D_UNSIGNED){
        return "unsigned";
      }else if(type==D_FLOAT){
        return "float";
      }else if(type==D_DOUBLE){
        return "double";
      }else{
        return "typedef";
      }
    }

		virtual void print(std::ostream &dst) const override
		{
			switch(type){
				case D_CHAR:     dst << "char";          break;
				case D_UCHAR:    dst << "unsigned char"; break;
				case D_INT:      dst << "int";           break;
				case D_FLOAT:    dst << "float";         break;
				case D_DOUBLE:   dst << "double";        break;
				case D_UNSIGNED: dst << "unsigned";      break;
        case TYPEDEF:    dst << "typedef";       break;
				case TYPEID:     dst << "typeid";        break;
				case STRUCTS:    dst << "struct";        break;
				case ENUMS:      dst << "enum";          break;
			}
			dst << std::endl;
		}

	  virtual void print_mips(std::ostream &dst, Context& context) const override
		{
      if(type==STRUCTS){
        struct_declr->print_mips(dst, context);
      }else if(type==ENUMS){
        enum_declr->print_mips(dst, context);
      }
    }
};

class StructDeclaration : public FunnyClass
{
	protected:
		PrimitiveType* type;
		std::vector<Declarator*>* declarators;
	public:
		StructDeclaration(PrimitiveType* _type, std::vector<Declarator*>* _declarators)
		: type(_type), declarators(_declarators) {}

    ~StructDeclaration()
    {
      delete type;
      for(int i=0; i<declarators->size(); i++){
        delete declarators->at(i);
      }
      delete declarators;
    }

    virtual DeclarationEnum get_type() const
    {
      return type->get_type();
    }

    virtual std::vector<Declarator*>* get_declrs() const
    {
      return declarators;
    }

    virtual std::string get_id() const override { return "struct_declrion"; }

    virtual void print(std::ostream &dst) const override
    {
      type->print(dst);
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {}
};

class FunctionDeclarator : public FunnyClass
{
	protected:
		FunnyClass* declr;
	public:
		FunctionDeclarator(FunnyClass* _declr)
		: declr(_declr) {}

    virtual std::string get_id() const
    {
      return declr->get_id();
    }

    virtual void print(std::ostream &dst) const override {}
		virtual void print_mips(std::ostream &dst, Context& context) const override {}
};

class FunctionDefinition : public FunnyClass                   // complete
{
  protected:
		PrimitiveType* return_type;
		Declarator* declr;
		Statement* statement;
	public:
		FunctionDefinition(std::vector<PrimitiveType*>* _return_type, Declarator* _declr, Statement* _statement)
		: declr(_declr), statement(_statement)
    {
      if(_return_type->size()==1){
        return_type = _return_type->at(0);
      }else{
        return_type = new PrimitiveType(_return_type->at(0), _return_type->at(1));
      }
      delete _return_type;
    }

    ~FunctionDefinition()
    {
      delete return_type;
      delete declr;
      delete statement;
    }

    virtual std::string get_id() const override
    {
      return declr->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << declr->get_id();
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      context.add_function(return_type->get_id(), declr->get_id(), statement);
      std::string label = context.get_label_function(declr->get_id());
      std::string stack_size = context.get_stack_size(declr->get_id());

      dst << label << ":" << std::endl;
      dst << "addiu $sp,$sp,-" << stack_size << std::endl;
      dst << "sw $31," << std::to_string(std::stoi(stack_size)-4) << "($sp)" << std::endl;
      dst << "sw $fp," << std::to_string(std::stoi(stack_size)-8) << "($sp)" << std::endl;
      dst << "move $fp,$sp" << std::endl;

      statement->print_mips(dst, context);

      if(declr->get_id()==std::string("main")){ dst << "move $2,$0" << std::endl; }
      dst << "move $sp,$fp" << std::endl;
      dst << "lw $31," << std::to_string(std::stoi(stack_size)-4) << "($sp)" << std::endl;
      dst << "lw $fp," << std::to_string(std::stoi(stack_size)-8) << "($sp)" << std::endl;
      dst << "addiu $sp,$sp," << stack_size << std::endl;
      dst << "j $31" << std::endl;
      dst << "nop" << std::endl;
    }
};


class ArrayDeclarator : public FunnyClass
{
  protected:
		FunnyClass* declr;
		Expression* size;
	public:
		ArrayDeclarator(FunnyClass* _declr, Expression* _size=NULL)
		: declr(_declr), size(_size) {}

    ~ArrayDeclarator()
    {
      delete declr;
      if(size){ delete size; }
    }

    virtual std::string get_id() const override
    {
      return declr->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << "array ";
      if(size){ dst << size->evaluate(); }
    }

		virtual void print_mips(std::ostream &dst, Context& context) const override
    {}
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

    virtual std::string get_id() const override { return "init_list"; }
    virtual void print(std::ostream &dst) const override {}
		virtual void print_mips(std::ostream &dst, Context& context) const override {}
};

class InitDeclarator : public FunnyClass
{
	protected:
		Declarator* declr;
		Initializer* init;
	public:
		InitDeclarator(Declarator* _declr, Initializer* _init=NULL)
		: declr(_declr), init(_init) {}

    virtual std::string get_id() const override { return "init_declr"; }
    virtual void print(std::ostream &dst) const override {}
		virtual void print_mips(std::ostream &dst, Context& context) const override {}
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

    virtual std::string get_id() const override { return "declrion"; }
    virtual void print(std::ostream &dst) const override {}
		virtual void print_mips(std::ostream &dst, Context& context) const override {}
};


class Root : public Node                                       // complete
{
	protected:
		std::vector<Node*>* translational_unit;
	public:
		Root(std::vector<Node*>* _translational_unit)
		: translational_unit(_translational_unit) {}

    virtual void print(std::ostream &dst) const
		{
			dst << "root";
		}

	  virtual void print_mips(std::ostream &dst, Context& context) const override
    {
      for(int i=0; i<translational_unit->size(); i++){
        (translational_unit->at(i))->print_mips(dst, context);
      }
    }
};

#endif
