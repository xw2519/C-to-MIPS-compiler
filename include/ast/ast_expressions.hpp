#ifndef EXPRESSIONS
#define EXPRESSIONS

#include "ast_node.hpp"

enum ExpressionEnum { INT, CHAR, UNSIGNED, FLOAT, DOUBLE, STRING_LITERAL, DOT, ARROW, INCREMENT, DECREMENT, REFERENCE, DEREFERENCE,
                      PLUS, MINUS, BITWISE_NOT, LOGICAL_NOT, SIZEOF, MULTIPLY, DIVIDE, MODULO, SHIFT_LEFT, SHIFT_RIGHT,
                      LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
                      DIV_ASSIGN, MOD_ASSIGN, SHIFT_LEFT_ASSIGN, SHIFT_RIGHT_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN, STRUCT };


/* -------------------------------- Expression Base Class -------------------------------- */

class Expression
{
protected:
  ExpressionEnum type;

public:
    virtual ~Expression()
    {}
    virtual double evaluate() const =0;
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
        value = std::stoi(value)
      }
    }

    virtual double evaluate() const override
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
      dst << "li " << destReg << ",0" << std::endl;
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

    virtual double evaluate() const override
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
      dst << "li " << destReg << ",0" << std::endl;
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
    : value(_ID)
    {
      type = context.get_type(value);
    }

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
        dst << "li " << destReg << "," << addr << std::endl;
      }
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      std::string base_addr = context.id_to_addr(value);                                   // get address associated with identifier
      std::string addr, instr;
      int total_words;

      if(context.write(value)){ instr = "s"; }                        // form instruction for load/store and int/float
      else{ instr = "l"; }
      if((type==FLOAT) || (type==DOUBLE)){ instr += "wc1 "; }
      else{ instr += "w "; }

      if(type==STRUCT){ total_words = context.size_in_words(value); }                 // get total size in words
      else if(type==DOUBLE){ total_words = 2; }
      else{ total_words = 1; }

      if((context.write(value)){ std::string freeReg = context.alloc_reg(INT); }        // choose register to store address
      else{ std::string freeReg = destReg; }

      if((context.check_global(value))){                                             // write/read global variable
        dst << "lui " << freeReg << ",%hi(" << base_addr << ")" << std::endl;
        dst << "addiu " << freeReg << "," << freeReg << ",%lo(" << base_addr << ")" << std::endl;
        for(int i=(total_words-1); i>=0; i--){
          dst << instr << destReg << "," << (4*i) << "(" << freeReg << ")" << std::endl;
          destReg = context.next_reg(destReg);
        }
      }else{                                                                           // read/write local variable
        for(int i=(total_words-1); i>=0; i--){
          addr = to_string(stoi(base_addr)+4*i);
          dst << instr << destReg << "," << addr << "($fp)" << std::endl;
          destReg = context.next_reg(destReg);
        }
      }
      if(context.write(value)){ context.dealloc_reg(freeReg); }
		}
};

class StringLiteral : public Expression                        // complete
{
  protected:
		std::string value;

	public:
		String_Literal(std::string &_string) : value(_string) {}

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
      std::string addr = context.get_string_addr(value);
      dst << "lui " << destReg << ",%hi(" << addr << ")" << std::endl;
      dst << "addiu " << destReg << "," << destReg << ",%lo(" << addr << ")" << std::endl;
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const
		{
      std::string addr = context.get_string_addr(value);
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
      std::string freeReg = context.alloc_reg(INT);
      postfix_expr->print_mips(dst, context, destReg);
      assignment_expr->print_mips(dst, context, freeReg);

      if(context.pointed_type(postfix_expr->get_id())==DOUBLE)
        { dst << "sll " << freeReg << "," << freeReg << ",1" << std::endl; }

      dst << "addu " << destReg << "," << destReg << "," << freeReg << std::endl;
      dst << "sll " << destReg << "," << destReg << ",2" << std::endl;

      context.dealloc_reg(freeReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      ExpressionEnum array_type = context.pointed_type(postfix_expr->get_id());
      int total_words = context.size_in_words_pointed(postfix_expr->get_id());
      std::string instr;

      std::string freeReg1 = context.alloc_reg(INT);
      std::string freeReg2 = context.alloc_reg(INT);

      postfix_expr->print_mips(dst, context, freeReg1);
      assignment_expr->print_mips(dst, context, freeReg2);

      if(context.write(postfix_expr->get_id())){ instr = "s"; }
      else{ instr = "l"; }
      if((array_type==FLOAT) || (array_type==DOUBLE)){ instr += "wc1 "; }
      else{ instr += "w "; }

      dst << "sll " << freeReg2 << "," << freeReg2 << ",2" << std::endl;
      for(int i=0; i<total_words; i++){
        dst << "addu " << freeReg1 << "," << freeReg1 << "," << freeReg2 << std::endl;
      }

      for(int i=total_words-1; i>=0; i--){
        dst << instr << destReg << "," << (4*i) << "("  << freeReg1 << ")" << std::endl;
        destReg = context.next_reg(destReg);
      }

      context.dealloc_reg(freeReg1);
      context.dealloc_reg(freeReg2);
    }
};

class FunctionPostfixExpression : public Expression            // works if args are integral type, as in tests
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

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if(arguments->size()>4){ std::string freeReg = context.alloc_reg(INT); }
      std::string paramReg = "$4";

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

class PostfixExpression : public Expression                    // works with integral type, need to finish dot and arrow for other types
{
  protected:
    Expression* postfix_expr;
    std::string value;
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
        std::string addr = context.member_to_addr(postfix_expr->get_id(),value);
        std::string freeReg;

        if(context.write(postfix_expr->get_id())){ freeReg = context.alloc_reg(INT); }
        else{ freeReg = destReg; }

        if(type==DOT){ postfix_expr->mips_address(dst, context, freeReg); }
        else{ postfix_expr->print_mips(dst, context, freeReg); }
        dst << "addiu " << freeReg << "," << freeReg << "," << addr << std::endl;

        if(context.write(postfix_expr->get_id())){
          dst << "sw " << destReg << "," << ",0(" << freeReg << ")" << std::endl;
          context.dealloc_reg(freeReg);
        }else{ dst << "lw " << destReg << "," << ",0(" << freeReg << ")" << std::endl; }

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

class UnaryExpression : public Expression                      // works with integral type, need to finish unary minus and dereferencing for other types
{
	protected:
		Expression* expr;
    Declaration* declr;

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
        if(context.write(expr->get_id())){
          std::string freeReg = context.alloc_reg(INT);
          expr->print_mips(dst, context, freeReg);
          dst << "sw " << destReg << ",0(" << freeReg << ")" << std::endl;
          context.dealloc_reg(freeReg);
        }else{
          expr->print_mips(dst, context, destReg);
          dst << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
        }
      }else if(type==SIZEOF){
        if(expr){
          dst << "li " << destReg << "," << context.size_of(expr.get_id()) << std::endl;
        }else{
          dst << "li " << destReg << "," << context.size_of(declr) << std::endl;
        }
      }else if(type==LOGICAL_NOT){
        expr->print_mips(dst, context, destReg);
        dst << "sltu " << destReg << "," << destReg << ",1" << std::endl;
        dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
      }else if(type==MINUS){
        expr->print_mips(dst, context, destReg);
        dst << "subu " << destReg << ",$0," << destReg << std::endl;
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

class MultiplicativeExpression : public Operator               // works with integral type, NEED TO FINISH
{

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
      if(context.get_type(postfix_expr->get_id())==FLOAT){
        std::string freeReg = context.alloc_reg(FLOAT);
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        if(type==MULTIPLY){ dst << "mul.s " << destReg << "," << freeReg << "," << destReg << std::endl; }
        else{ "div.s " << destReg << "," << freeReg << "," << destReg << std::endl; }
        context.dealloc(freeReg);
      }else if(context.get_type(postfix_expr->get_id())==DOUBLE){
        /* Do this with doubles?!?!??! no thanks... */
      }else{
        std::string freeReg = context.alloc_reg(INT);
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        if(type==MULTIPLY){ dst << "mult " << destReg << "," << freeReg << std::endl; }
        else{
          dst << "bne " << freeReg << ",$0,1f" << std::endl;
          if(context.get_type(postfix_expr->get_id())==UNSIGNED){ dst << "div $0," << destReg << "," << freeReg << std::endl; }
          else{ dst << "divu $0," << destReg << "," << freeReg << std::endl; }
          dst << "break 7" << std::endl;
          dst << "mfhi " << destReg << std::endl;
          if(type==DIVIDE){ dst << "mflo " << destReg << std::endl; }
        }
        context.dealloc(freeReg);
      }
    }
};

class AdditiveExpression : public Operator                     // works with integral type, NEED TO FINISH
{

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
      if(type==PLUS){ std::string instr = "add"; }
      else{ std::string instr = "sub"; }
      if((context.get_type(postfix_expr->get_id())==FLOAT) || (context.get_type(postfix_expr->get_id())==DOUBLE)){ instr += ".s"; }
      else{ instr += "u"; }

      if(context.get_type(postfix_expr->get_id())==DOUBLE){
        // do double
      }else{
        if(context.get_type(postfix_expr->get_id())==FLOAT){ std::string freeReg = context.alloc_reg(FLOAT); }
        else{ std::string freeReg = context.alloc_reg(INT); }
        left->print_mips(dst, context, destReg);
        right->print_mips(dst, context, freeReg);
        dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
        if(context.get_type(postfix_expr->get_id())==CHAR){
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
      if(type==SHIFT_LEFT){ std::string instr = "sll"; }
      else if(context.get_type(postfix_expr->get_id())==UNSIGNED){ std::string instr = "srl"; }
      else{ std::string instr = "sra"; }

      std::string freeReg = context.alloc_reg(INT);
      left->print_mips(dst, context, destReg);
      right->print_mips(dst, context, freeReg);

      dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
      if(context.get_type(postfix_expr->get_id())==CHAR)
        { dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl; }
      context.dealloc_reg(freeReg);
    }
};


/* -------------------------------- Relational Binary Expressions -------------------------------- */

class RelationalExpression : public Operator                   // works with integral type, NEED TO FINISH
{

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
      std::string freeReg = context.alloc_reg(INT);
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
        if(context.get_type(postfix_expr->get_id())==UNSIGNED){ std::string instr = "sltu "; }
        else{ std::string instr = "slt "; }
        dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
        if((type==GREATER_EQUAL) || (type==LESS_EQUAL))
          { dst << "xori " << destReg << "," << destReg << ",0x1" << std::endl; }
      }

      dst << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
      context.dealloc_reg(freeReg);
    }
};

class BitwiseExpression : public Operator                      // complete
{

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
      std::string freeReg = context.alloc_reg(INT);
      left->print_mips(dst, context, destReg);
      right->print_mips(dst, context, freeReg);

      if(type==BITWISE_AND){ std::string instr = "and "; }
      else if(type==BITWISE_OR){ std::string instr = "or "; }
      else{ std::string instr = "xor "; }

      dst << instr << destReg << "," << freeReg << "," << destReg << std::endl;
      context.dealloc_reg(freeReg);
    }
};


/* -------------------------------- Assignment Expression -------------------------------- */

class AssignmentExpression : public Expression
{
	protected:
		Expression* lvalue;
		Expression* expression;

	public:
		Assignment_Expression(ExpressionEnum _type, Expression* _lvalue, Expression* _expression)
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
    {}
};
