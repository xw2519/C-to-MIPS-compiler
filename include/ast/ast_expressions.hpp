#ifndef EXPRESSIONS
#define EXPRESSIONS

#include "ast_node.hpp"

enum ExpressionEnum { DOT, ARROW, INCREMENT, DECREMENT, REFERENCE, DEREFERENCE, BITWISE_AND, BITWISE_OR, BITWISE_XOR,
                      PLUS, MINUS, BITWISE_NOT, LOGICAL_NOT, SIZEOF, MULTIPLY, DIVIDE, MODULO, SHIFT_LEFT, SHIFT_RIGHT,
                      LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
                      DIV_ASSIGN, MOD_ASSIGN, SHIFT_LEFT_ASSIGN, SHIFT_RIGHT_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN,
                      INT, CHAR, UCHAR, UNSIGNED, FLOAT, DOUBLE, STRING_LITERAL, STRUCT };

class Declaration;
#include "ast_context.hpp"

/* -------------------------------- Expression Base Class -------------------------------- */

class Expression : public Node
{
protected:
  ExpressionEnum type;

public:
    virtual ~Expression()
    {}
    virtual std::string get_id() const =0;
    virtual void print(std::ostream &dst) const =0;
    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const =0;
    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const =0;
};


/* -------------------------------- Primitives -------------------------------- */

class IntegralConstant : public Expression                     // complete
{
  protected:
    long value;

	public:
		IntegralConstant(std::string &_value)
    {
      if((_value.back()=='u')||(_value.back()=='U')){
        type = UNSIGNED;
      }else if(_value.front()=='\''){
        type = CHAR;
      }else{
        type = INT;
      }

      if(type==CHAR){
        value = _value[_value.size()-2];
      }else if((_value[1] == 'x') || (_value[1] == 'X')){
        value = std::stoi(_value, 0, 16);
      }else if(_value.front() == '0'){
        value = std::stoi(_value, 0, 8);
      }else{
        value = std::stoi(_value);
      }
    }

    virtual double evaluate() const
    {
      return value;
    }

    virtual std::string get_id() const override
    {
      if(type==CHAR){ return std::string("char"); }
      else if(type==INT){ return std::string("int"); }
      else{ return std::string("unsigned"); }
    }

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      dst << "address of int const" << std::endl;
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      dst << "li " << destReg << "," << (value&&(0x00000000FFFF0000)) << std::endl;
      dst << "ori " << destReg << "," << destReg << ",0x" << std::hex << (value%65536) << std::dec << std::endl;
		}
};

class FloatConstant : public Expression                        // complete
{
  protected:
    double value;

	public:
    FloatConstant(std::string &_value)
    {
      if((_value.back()=='f') || (_value.back()=='F')){
        type = FLOAT;
      }else{
        type = DOUBLE;
      }

      value = std::stof(_value);
    }

    virtual double evaluate() const
    {
      return value;
    }

    virtual std::string get_id() const override
    {
      if(type==DOUBLE){ return std::string("double"); }
      else{ return std::string("float"); }
    }

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      dst << "address of float const" << std::endl;
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      std::string label = context.get_float_label(value);
      std::string freeReg = context.alloc_reg(INT);
      dst << "lui " << freeReg << ",%hi(" << label << ")" << std::endl;
      if(type==FLOAT){
        dst << "lwc1 " << destReg << ",%lo(" << label << ")(" << freeReg << ")" << std::endl;
      }else{
        std::string destReg2 = context.next_reg(destReg);
        dst << "lwc1 " << destReg << ",%lo(" << label << "+4)(" << freeReg << ")" << std::endl;
        dst << "lwc1 " << destReg2 << ",%lo(" << label << ")(" << freeReg << ")" << std::endl;
      }
      context.dealloc_reg(freeReg);
		}
};

class Identifier : public Expression                           // complete
{
  protected:
		std::string value;

	public:
		Identifier(std::string &_ID)
    : value(_ID) {}

    virtual std::string get_id() const override
    {
      return value;
    }

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string addr = context.id_to_addr(value);
      if(context.check_global(value)){
        dst << "lui " << destReg << ",%hi(" << addr << ")" << std::endl;
        dst << "addiu " << destReg << "," << destReg << ",%lo(" << addr << ")" << std::endl;
      }else{
        dst << "addiu " << destReg << ",$fp," << addr << std::endl;
      }
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      std::string base_addr = context.id_to_addr(value);                                   // get address associated with identifier
      std::string addr, instr;
      int total_words;
      ExpressionEnum id_type = context.get_type(value);

      if((id_type==FLOAT) || (id_type==DOUBLE)){ instr = "lwc1 "; }                     // form instruction for int/float
      else{ instr = "lw "; }

      if(id_type==STRUCT){ total_words = context.size_of(value); }                 // get total size in words
      else if(id_type==DOUBLE){ total_words = 2; }
      else{ total_words = 1; }

      if((context.check_global(value))){                                             // write/read global variable
        dst << "lui " << destReg << ",%hi(" << base_addr << ")" << std::endl;
        dst << "addiu " << destReg << "," << destReg << ",%lo(" << base_addr << ")" << std::endl;
        for(int i=(total_words-1); i>=0; i--){
          dst << instr << destReg << "," << (4*i) << "(" << destReg << ")" << std::endl;
          destReg = context.next_reg(destReg);
        }
      }else{                                                                           // read/write local variable
        for(int i=(total_words-1); i>=0; i--){
          addr = std::to_string(stoi(base_addr)+4*i);
          dst << instr << destReg << "," << addr << "($fp)" << std::endl;
          destReg = context.next_reg(destReg);
        }
      }
		}
};

class StringLiteral : public Expression                        // might need work for char arrays
{
  protected:
		std::string value;

	public:
		StringLiteral(std::string &_value) : value(_value) {}

    virtual std::string get_id() const override
    {
      return std::string("string");
    }

    virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string addr = context.get_string_label(value);
      dst << "lui " << destReg << ",%hi(" << addr << ")" << std::endl;
      dst << "addiu " << destReg << "," << destReg << ",%lo(" << addr << ")" << std::endl;
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const
		{
      std::string addr = context.get_string_label(value);
      dst << "lui " << destReg << ",%hi(" << addr << ")" << std::endl;
      dst << "addiu " << destReg << "," << destReg << ",%lo(" << addr << ")" << std::endl;
		}
};


/* -------------------------------- Post-fix Expression -------------------------------- */

class ArrayPostfixExpression : public Expression               // complete
{
  protected:
    Expression* postfix_expr;
    Expression* assignment_expr;

  public:
    ArrayPostfixExpression(Expression* _postfix_expr, Expression* _assignment_expr)
    : postfix_expr(_postfix_expr)
    , assignment_expr(_assignment_expr) {}

    ~ArrayPostfixExpression()
    {
      delete postfix_expr;
      delete assignment_expr;
    }

    virtual std::string get_id() const override
    {
      return postfix_expr->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << postfix_expr->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      int total_words = context.size_of_pointed(postfix_expr->get_id());
      std::string freeReg = context.alloc_reg(INT);

      postfix_expr->print_mips(dst, context, destReg);
      assignment_expr->print_mips(dst, context, freeReg);

      dst << "sll " << freeReg << "," << freeReg << ",2" << std::endl;
      for(int i=0; i<total_words; i++){
        dst << "addu " << destReg << "," << destReg << "," << freeReg << std::endl;
      }

      context.dealloc_reg(freeReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      ExpressionEnum array_type = context.get_type_pointed(postfix_expr->get_id());
      int total_words = context.size_of_pointed(postfix_expr->get_id());
      std::string instr;

      std::string freeReg = context.alloc_reg(INT);

      postfix_expr->print_mips(dst, context, freeReg);
      assignment_expr->print_mips(dst, context, destReg);

      if((array_type==FLOAT) || (array_type==DOUBLE)){ instr = "lwc1 "; }
      else{ instr = "lw "; }

      dst << "sll " << destReg << "," << destReg << ",2" << std::endl;
      for(int i=0; i<total_words; i++){
        dst << "addu " << freeReg << "," << freeReg << "," << destReg << std::endl;
      }

      for(int i=total_words-1; i>=0; i--){
        dst << instr << destReg << "," << (4*i) << "("  << freeReg << ")" << std::endl;
        destReg = context.next_reg(destReg);
      }

      context.dealloc_reg(freeReg);
    }
};

class FunctionPostfixExpression : public Expression            // works if args are integral type, enough for known tests
{
  protected:
    Expression* postfix_expr;
    std::vector<Expression*>* arguments;

  public:
    FunctionPostfixExpression(Expression* _postfix_expr) : postfix_expr(_postfix_expr) {}
    FunctionPostfixExpression(Expression* _postfix_expr, std::vector<Expression*>* _arguments)
    : postfix_expr(_postfix_expr)
    , arguments(_arguments) {}

    ~FunctionPostfixExpression()
    {
      delete postfix_expr;
      for(int i=0; i<arguments->size(); i++){
        delete arguments->at(i);
      }
      delete arguments;
    }

    virtual std::string get_id() const override
    {
      return postfix_expr->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << postfix_expr->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      dst << "address of function call" << std::endl;
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string freeReg;
      std::string paramReg = "$4";
      if(arguments->size()>4){ freeReg = context.alloc_reg(INT); }

      if(arguments){
        for(int i=0; i<arguments->size(); i++){
          if(i<4){
            arguments->at(i)->print_mips(dst, context, paramReg);
            paramReg = context.next_reg(paramReg);
          }else{
            arguments->at(i)->print_mips(dst, context, freeReg);
            dst << "sw " << freeReg << "," << (4*i) << "($sp)" << std::endl;
          }
        }
      }

      dst << "jal " << context.id_to_addr(postfix_expr->get_id()) << std::endl;
      dst << "nop" << std::endl;
      if(arguments->size()>4){ context.dealloc_reg(freeReg); }
    }
};

class PostfixExpression : public Expression                    // complete
{
  protected:
    Expression* postfix_expr;
    std::string value;
    ExpressionEnum type;
	public:
		PostfixExpression(ExpressionEnum _type, Expression* _postfix_expr)
    : type(_type)
    , postfix_expr(_postfix_expr) {}
    PostfixExpression(ExpressionEnum _type, Expression* _postfix_expr, std::string &_value)
    : type(_type)
    , postfix_expr(_postfix_expr)
    , value(_value) {}

    ~PostfixExpression()
    {
      delete postfix_expr;
    }

    virtual std::string get_id() const override
    {
      std::string out = postfix_expr->get_id();
      if(type==DOT){
        out += ".";
        out += value;
      }else if(type==ARROW){
        out += ">";
        out += value;
      }
      return out;
    }

    virtual void print(std::ostream &dst) const override
    {
      std::string out = postfix_expr->get_id();
      if(type==DOT){
        out += ".";
        out += value;
      }else if(type==ARROW){
        out += ">";
        out += value;
      }
      dst << out;
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if((type==DOT) || (type==ARROW)){
        std::string addr = context.member_to_addr(postfix_expr->get_id(),value);
        if(type==DOT){ postfix_expr->mips_address(dst, context, destReg); }
        else{ postfix_expr->print_mips(dst, context, destReg); }
        dst << "addiu " << destReg << "," << destReg << "," << addr << std::endl;
      }else{
        postfix_expr->mips_address(dst, context, destReg);
      }
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if((type==DOT) || (type==ARROW)){
        std::string addr = context.member_to_addr(postfix_expr->get_id(),value);        // address of member relative to struct
        ExpressionEnum member_type = context.get_type_member(postfix_expr->get_id(),value);    // type of member
        int total_words = context.size_of_member(postfix_expr->get_id(),value);       // size of member
        std::string instr, freeReg;
        if((member_type==FLOAT) || (member_type==DOUBLE)){ instr = "lwc1 "; }
        else{ instr = "lw "; }
        if(total_words==1){ freeReg = destReg; }
        else{ freeReg = context.alloc_reg(INT); }

        if(type==DOT){ postfix_expr->mips_address(dst, context, freeReg); }               // get address of member
        else{ postfix_expr->print_mips(dst, context, freeReg); }
        dst << "addiu " << freeReg << "," << freeReg << "," << addr << std::endl;

        for(int i=(total_words-1); i>=0; i--){
          dst << "lw " << destReg << "," << (4*i) << "(" << freeReg << ")" << std::endl;
          destReg = context.next_reg(destReg);
        }

        if(total_words>1){ context.dealloc_reg(freeReg); }
      }else{
        std::string freeReg1 = context.alloc_reg(INT);
        std::string freeReg2 = context.alloc_reg(INT);
        postfix_expr->print_mips(dst, context, destReg);
        postfix_expr->mips_address(dst, context, freeReg1);
        if(type==INCREMENT){
          dst << "addiu " << freeReg2 << "," << destReg << ",1" << std::endl;
        }else{
          dst << "addiu " << freeReg2 << "," << destReg << ",-1" << std::endl;
        }
        dst << "sw " << freeReg2 << ",0(" << freeReg1 << ")" << std::endl;
        context.dealloc_reg(freeReg1);
        context.dealloc_reg(freeReg2);
      }
    }
};

/* -------------------------------- Unary Expression -------------------------------- */

class UnaryExpression : public Expression                      // complete
{
	protected:
		Expression* expr;
    Declaration* declr;
    ExpressionEnum type;
	public:
		UnaryExpression(ExpressionEnum _type, Expression* _expr)
    : type(_type)
    , expr(_expr) {}
    UnaryExpression(ExpressionEnum _type, Declaration* _declr)
    : type(_type)
    , declr(_declr) {}

    ~UnaryExpression()
    {
      delete expr;
    }

    virtual std::string get_id() const override
    {
      return expr->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << expr->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if(type==DEREFERENCE){
        expr->print_mips(dst, context, destReg);
      }else{
        expr->mips_address(dst, context, destReg);
      }
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if((type==INCREMENT) || (type==DECREMENT)){
        std::string freeReg = context.alloc_reg(INT);
        expr->print_mips(dst, context, destReg);
        expr->mips_address(dst, context, freeReg);
        if(type==INCREMENT){
          dst << "addiu " << destReg << "," << destReg << ",1" << std::endl;
        }else{
          dst << "addiu " << destReg << "," << destReg << ",-1" << std::endl;
        }
        dst << "sw " << destReg << ",0(" << freeReg << ")" << std::endl;
        context.dealloc_reg(freeReg);
      }else if(type==REFERENCE){
        expr->mips_address(dst, context, destReg);
      }else if(type==DEREFERENCE){
        int total_words = context.size_of_pointed(expr->get_id());
        ExpressionEnum pointed_type = context.get_type_pointed(expr->get_id());
        std::string freeReg, instr;
        if(total_words>1){ freeReg = context.alloc_reg(INT); }
        else{ freeReg = destReg; }
        if((pointed_type==FLOAT) || (pointed_type==DOUBLE)){ instr = "lwc1 "; }
        else{ instr = "lw "; }

        expr->print_mips(dst, context, freeReg);
        for(int i=(total_words-1); i>=0; i--){
          dst << instr << destReg << "," << (4*i) << "(" << freeReg << ")" << std::endl;
          destReg = context.next_reg(destReg);
        }

        if(total_words>1){ context.dealloc_reg(freeReg); }
      }else if(type==SIZEOF){
        if(expr){
          dst << "li " << destReg << "," << context.size_of(expr->get_id()) << std::endl;
        }else{
          dst << "li " << destReg << "," << context.size_of(declr) << std::endl;
        }
      }else if(type==LOGICAL_NOT){
        expr->print_mips(dst, context, destReg);
        dst << "sltu " << destReg << "," << destReg << ",1" << std::endl;
        dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
      }else if(type==MINUS){
        ExpressionEnum pointed_type = context.get_type_pointed(expr->get_id());
        ExpressionEnum expr_type = context.get_type(expr->get_id());
        expr->print_mips(dst, context, destReg);

        if((pointed_type==FLOAT) || (pointed_type==DOUBLE)){
          std::string freeReg = context.alloc_reg(INT);
          dst << "li " << freeReg << ",-2147483648";
          if(pointed_type==DOUBLE){ destReg = context.next_reg(destReg); }
          dst << "xor " << destReg << "," << freeReg << "," << destReg << std::endl;
          context.dealloc_reg(freeReg);
        }else{
          dst << "subu " << destReg << ",$0," << destReg << std::endl;
        }
      }else{
        expr->print_mips(dst, context, destReg);
        dst << "nor " << destReg << ",$0," << destReg << std::endl;
      }
    }
};


/* -------------------------------- Operator Expression -------------------------------- */

class Operator : public Expression                             // complete
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Operator(Expression* _left, Expression* _right)
    : left(_left)
    , right(_right) {}
};

/* -------------------------------- Arithmetic Expression -------------------------------- */

class MultiplicativeExpression : public Operator               // works with all but doubles, NEED TO FINISH
{
  protected:
    ExpressionEnum type;
	public:
		MultiplicativeExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left,_right)
    , type(_type) {}

    ~MultiplicativeExpression()
    {
      delete left;
      delete right;
    }

    virtual std::string get_id() const override
    {
      return left->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << left->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      left->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if(context.get_type(left->get_id())==FLOAT){
        std::string freeReg = context.alloc_reg(FLOAT);
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        if(type==MULTIPLY){ dst << "mul.s " << destReg << "," << freeReg << "," << destReg << std::endl; }
        else{ dst << "div.s " << destReg << "," << freeReg << "," << destReg << std::endl; }
        context.dealloc_reg(freeReg);
      }else if(context.get_type(left->get_id())==DOUBLE){
        /* Do this with doubles?!?!??! no thanks... */
      }else{
        std::string freeReg = context.alloc_reg(INT);
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        if(type==MULTIPLY){ dst << "mult " << destReg << "," << freeReg << std::endl; }
        else{
          dst << "bne " << freeReg << ",$0,1f" << std::endl;
          if(context.get_type(left->get_id())==UNSIGNED){ dst << "divu $0," << destReg << "," << freeReg << std::endl; }
          else{ dst << "div $0," << destReg << "," << freeReg << std::endl; }
          dst << "break 7" << std::endl;
          dst << "mfhi " << destReg << std::endl;
        }
        if((type==DIVIDE) || (type==MULTIPLY)){ dst << "mflo " << destReg << std::endl; }
        if(context.get_type(left->get_id())==CHAR)
          { dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl; }
        context.dealloc_reg(freeReg);
      }
    }
};

class AdditiveExpression : public Operator                     // works with integral type, NEED TO FINISH
{
  protected:
    ExpressionEnum type;
	public:
		AdditiveExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~AdditiveExpression()
    {
      delete left;
      delete right;
    }

    virtual std::string get_id() const override
    {
      return left->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << left->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      left->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string instr, freeReg;
      if(type==PLUS){ instr = "add"; }
      else{ instr = "sub"; }
      if((context.get_type(left->get_id())==FLOAT) || (context.get_type(left->get_id())==DOUBLE)){ instr += ".s"; }
      else{ instr += "u"; }

      if(context.get_type(left->get_id())==DOUBLE){
        // do double
      }else{
        if(context.get_type(left->get_id())==FLOAT){ freeReg = context.alloc_reg(FLOAT); }
        else{ freeReg = context.alloc_reg(INT); }
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        dst << instr << destReg << "," << destReg << "," << freeReg << std::endl;
        if(context.get_type(left->get_id())==CHAR){
          dst << "move " << freeReg << ",$0" << std::endl;
          dst << "li " << freeReg << ",255" << std::endl;
          dst << "and " << destReg << "," << freeReg << "," << destReg << std::endl;
        }
        context.dealloc_reg(freeReg);
      }
    }
};

class ShiftExpression : public Operator                        // complete
{
  protected:
    ExpressionEnum type;
	public:
		ShiftExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~ShiftExpression()
    {
      delete left;
      delete right;
    }

    virtual std::string get_id() const override
    {
      return left->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << left->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      left->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string instr;
      if(type==SHIFT_LEFT){ instr = "sll"; }
      else if(context.get_type(left->get_id())==UNSIGNED){ instr = "srl"; }
      else{ instr = "sra"; }

      std::string freeReg = context.alloc_reg(INT);
      left->print_mips(dst, context, destReg);
      right->print_mips(dst, context, freeReg);

      dst << instr << destReg << "," << destReg << "," << freeReg << std::endl;
      if(context.get_type(left->get_id())==CHAR)
        { dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl; }
      context.dealloc_reg(freeReg);
    }
};


/* -------------------------------- Relational Binary Expressions -------------------------------- */

class RelationalExpression : public Operator                   // works with integral type, NEED TO FINISH
{
  protected:
    ExpressionEnum type;
	public:
		RelationalExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~RelationalExpression()
    {
      delete left;
      delete right;
    }

    virtual std::string get_id() const override
    {
      return left->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << left->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      left->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string freeReg = context.alloc_reg(INT), instr;
      if((type==LESS) || (type==GREATER_EQUAL)){
        left->print_mips(dst, context, freeReg);
        right->print_mips(dst, context, destReg);
      }else{
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
      }

      if((type==EQUAL) || (type==NOT_EQUAL)){
        dst << "xor " << destReg << "," << freeReg << "," << destReg << std::endl;
        if(type==EQUAL){ dst << "sltu " << destReg << "," << destReg << ",1" << std::endl; }
        else{ dst << "sltu " << destReg << ",$0," << destReg << std::endl; }
      }else{
        if(context.get_type(left->get_id())==UNSIGNED){ instr = "sltu "; }
        else{ instr = "slt "; }
        dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
        if((type==GREATER_EQUAL) || (type==LESS_EQUAL))
          { dst << "xori " << destReg << "," << destReg << ",0x1" << std::endl; }
      }

      dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
      context.dealloc_reg(freeReg);
    }
};

class BitwiseExpression : public Operator                      // VERY WRONG, LOGICAL AND OP AND OR OP, NOT BITWISE
{
  protected:
    ExpressionEnum type;
	public:
		BitwiseExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~BitwiseExpression()
    {
      delete left;
      delete right;
    }

    virtual std::string get_id() const override
    {
      return left->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << left->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      left->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string freeReg = context.alloc_reg(INT), instr;
      left->print_mips(dst, context, destReg);
      right->print_mips(dst, context, freeReg);

      if(type==BITWISE_AND){ instr = "and "; }
      else if(type==BITWISE_OR){ instr = "or "; }
      else{ instr = "xor "; }

      dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
      context.dealloc_reg(freeReg);
    }
};


/* -------------------------------- Assignment Expression -------------------------------- */

class AssignmentExpression : public Expression                 // should work for ints and int operations
{
	protected:
		Expression* lvalue;
		Expression* expression;
    ExpressionEnum type;

	public:
		AssignmentExpression(ExpressionEnum _type, Expression* _lvalue, Expression* _expression)
    : type(_type)
    , lvalue(_lvalue)
    , expression(_expression) {}

    ~AssignmentExpression()
    {
      delete lvalue;
      delete expression;
    }

    virtual std::string get_id() const override
    {
      return lvalue->get_id();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << lvalue->get_id();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      lvalue->mips_address(dst, context, destReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      bool float_type = ((context.get_type(lvalue->get_id())==FLOAT) || (context.get_type(lvalue->get_id())==DOUBLE));
      bool unsigned_type = (context.get_type(lvalue->get_id())==UNSIGNED);
      std::string freeReg, instr, op;
      int total_words;

      if((context.get_type(lvalue->get_id())==FLOAT) || (context.get_type(lvalue->get_id())==DOUBLE)){ instr = "swc1 "; }
      else{ instr = "sw "; }
      if(type==XOR_ASSIGN){ op = "xor "; }
      else if(type==OR_ASSIGN){ op = "or "; }
      else if(type==AND_ASSIGN){ op = "and "; }
      else if(type==SHIFT_LEFT_ASSIGN){ op = "sll "; }
      else if(type==SHIFT_RIGHT_ASSIGN){ if(unsigned_type){ op = "srl "; }else{ op = "sra "; } }
      else if(type==MOD_ASSIGN){ if(unsigned_type){ op = "divu "; }else{ op = "div "; } }
      else if(type==ADD_ASSIGN){ if(float_type){ op = "add.s "; }else{ op = "addu "; } }
      else if(type==SUB_ASSIGN){ if(float_type){ op = "sub.s "; }else{ op = "subu "; } }
      else if(type==MUL_ASSIGN){ if(float_type){ op = "mul.s "; }else{ op = "mult "; } }
      else if(type==DIV_ASSIGN){ if(float_type){ op = "div.s "; }else if(unsigned_type){ op = "divu "; }else{ op = "div "; } }

      if(type==ASSIGN){
        freeReg = context.alloc_reg(INT);
        total_words = context.size_of(lvalue->get_id());
        lvalue->mips_address(dst, context, freeReg);
        expression->print_mips(dst, context, destReg);
        for(int i=0; i<total_words; i++){
          dst << instr << destReg << "," << (4*i) << "(" << freeReg << ")" << std::endl;
          destReg == context.next_reg(destReg);
        }
      }else{
        freeReg = context.alloc_reg(INT);
        lvalue->print_mips(dst, context, destReg);
        expression->print_mips(dst, context, freeReg);
        dst << op << destReg << "," << destReg << "," << freeReg << std::endl;
        lvalue->mips_address(dst, context, freeReg);
        dst << instr << destReg << ",0(" << freeReg << ")" << std::endl;
      }


    }
};

#endif
