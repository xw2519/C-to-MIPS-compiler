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

class Constant : public Expression
{
	public:
    virtual double evaluate() const override =0;
    virtual std::string get_id() const override =0;
		virtual void print(std::ostream &dst) const override =0;
    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override =0;
		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override =0;
};

class IntegralConstant : public Constant
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

class FloatConstant : public Constant
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
      std::string label = context.make_float_label(value);
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

class StringLiteral : public Expression
{
  protected:
		std::string value;

	public:
		String_Literal(std::string &_string) : value(_string) {}

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual std::string get_id() const override
    {
      return std::string("string");
    }

		virtual void print_mips_array(std::ostream &dst, Context &context, std::string destReg) const
		{
      char current;
      int addr = context.help_me();

      for(int i=1; i<value.size(); i++){
        dst << "li " << destReg << "," << ((256*out[0]+out[1])*65536) << std::endl;
        dst << "ori " << destReg << "," << destReg << ",0x" << std::hex << 256*out[2]+out[3] << std::dec << std::endl;
        dst << "sw " << destReg << "," << addr << "($fp)" << std::endl;
        addr += 4;
      }
		}

    virtual void print_mips_pointer(std::ostream &dst, Context &context, std::string destReg) const
		{
      std::string label = context.make_literal_label(value);
      dst << "lui " << destReg << ",%hi(" << label << ")" << std::endl;
      dst << "addiu " << destReg << "," << destReg << ",%lo(" << label << ")" << std::endl;
		}
};

class Identifier : public Expression
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
      std::string addr = context.id_to_addr(value);
      if(type==DOUBLE){ std::string destReg2 = context.next_reg(destReg); }
      if(context.write(value)){ std::string instr = "s"; }
      else{ std::string instr = "l"; }
      if((type==FLOAT) || (type==DOUBLE)){ instr += "wc1 "; }
      else{ instr += "w "; }
      if(type==STRUCT){ int total_words = context.size_of(value); }

      if((context.write(value)){ std::string freeReg = context.alloc_reg(INT); }
      else{ std::string freeReg = destReg; }

      if((context.check_global(value))){                                             // write/read global variable
        dst << "lui " << freeReg << ",%hi(" << addr << ")" << std::endl;
        dst << "addiu " << freeReg << "," << freeReg << ",%lo(" << addr << ")" << std::endl;
        if(type==DOUBLE){
          dst << instr << destReg2 << ",0(" << freeReg << ")" << std::endl;
          dst << instr << destReg << ",4(" << freeReg << ")" << std::endl;
        }else if(type==STRUCT){
          for(int i=0; i<total_words; i++){
            dst << instr << destReg << "," << i << "(" << freeReg << ")" << std::endl;
            destReg = context.next_reg(destReg);
          }
        }else{
          dst << instr << destReg << ",0(" << freeReg << ")" << std::endl;
        }
      }else{                                                                           // read/write local variable
        if(type==DOUBLE){
          dst << instr << destReg2 << "," << addr << "($fp)" << std::endl;
          dst << instr << destReg << "," << (std::stoi(addr)+4) << "($fp)" << std::endl;
        }else if(type==STRUCT){
          for(int i=0; i<total_words; i++){
            dst << instr << destReg << "," << addr << "($fp)" << std::endl;
            destReg = context.next_reg(destReg);
            addr = to_string(stoi(addr)+4);
          }
        }else{
          dst << instr << destReg << "," << addr << "($fp)" << std::endl;
        }
      }
      if(context.write(value)){ context.dealloc_reg(freeReg); }
		}
};


/* -------------------------------- Post-fix Expression -------------------------------- */

class ArrayPostfixExpression : public Expression
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
      dst << postfix_expr->evaluate();
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string freeReg = context.alloc_reg(INT);
      assignment_expr->print_mips(dst, context, freeReg);
      postfix_expr->print_mips(dst, context, destReg);

      if(context.pointed_type(postfix_expr->get_id())==DOUBLE){
        dst << "sll " << freeReg << "," << freeReg << ",1" << std::endl;
        dst << "addu " << destReg << "," << destReg << "," << freeReg << std::endl;
        dst << "sll " << destReg << "," << destReg << ",2" << std::endl;
      }else if(context.pointed_type(postfix_expr->get_id())==CHAR){
        dst << "sll " << destReg << "," << destReg << ",2" << std::endl;
        dst << "addu " << destReg << "," << destReg << "," << freeReg << std::endl;
      }else{
        dst << "addu " << destReg << "," << destReg << "," << freeReg << std::endl;
        dst << "sll " << destReg << "," << destReg << ",2" << std::endl;
      }

      context.dealloc_reg(freeReg);
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::string freeReg1 = context.alloc_reg(INT);
      assignment_expr->print_mips(dst, context, freeReg1);

      std::string freeReg2 = context.alloc_reg(INT);
      postfix_expr->print_mips(dst, context, freeReg2);

      if(context.write(postfix_expr->get_id())){ std::string instr = "s"; }
      else{ std::string instr = "l"; }
      if((context.pointed_type(postfix_expr->get_id())==FLOAT) || (context.pointed_type(postfix_expr->get_id())==DOUBLE)){ instr += "wc1 "; }
      else{ instr += "w "; }

      if(context.pointed_type(postfix_expr->get_id())==DOUBLE)
        { dst << "sll " << freeReg1 << "," << freeReg1 << ",1" << std::endl; }
      dst << "addu " << freeReg1 << "," << freeReg1 << "," << freeReg2 << std::endl;
      dst << "sll " << freeReg1 << "," << freeReg1 << ",2" << std::endl;

      if(context.pointed_type(postfix_expr->get_id())==DOUBLE){
        std::string destReg2 = context.next_reg(destReg);
        dst << instr << destReg << ",4("  << freeReg1 << ")" << std::endl;
        dst << instr << destReg2 << ",0("  << freeReg1 << ")" << std::endl;
      }else{
        dst << instr << destReg << ",0(" << freeReg1 << ")" << std::endl;
      }

      context.dealloc_reg(freeReg1);
      context.dealloc_reg(freeReg2);
    }
};

class FunctionPostfixExpression : public Expression
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
      return postfix_expr->get_value();
    }

    virtual void print(std::ostream &dst) const override
    {
      dst << postfix_expr->evaluate();
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::vector<ExpressionEnum> arg_types = context.get_arg_types(postfix_expr->evaluate());
      std::string freeReg = context.alloc_reg(INT);
      std::string paramReg = "$";
      int float_param_count = 0;

      if(arguments){                                     // if statement handles loading args
        for(int i=0; i<arguments->size(); i++){             // iterate over args
          if((arg_types[i] == FLOAT) || (arg_types[i] == DOUBLE)){      // floats go to coprocessor
            paramReg.push_back('f');
            paramReg.push_back(std::to_string(2*float_param_count));
            arguments->at(i)->print_mips(dst, context, paramReg);
            float_param_count++;
            paramReg.pop_back();
            paramReg.pop_back();
          }else{
            if((i+float_param_count)<4){                 // check if parameter registers are occupied
              paramReg.push_back(std::to_string(4+i+float_param_count));
              arguments->at(i)->print_mips(dst, context, paramReg);
              paramReg.pop_back();
            }else{                                               // store args in stack if necessary
              arguments->at(i)->print_mips(dst, context, freeReg);
              dst << "sw " << freeReg << "," << (4*(i+float_param_count)) << "($sp)" << std::endl;
            }
          }
        }
      }
      dst << "jal " << postfix_expr->evaluate() << std::endl;
      dst << "nop" << std::endl;
      context.dealloc_reg(freeReg);
    }
};

class PostfixExpression : public Expression
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
      std::string out = postfix_expr->evaluate();
      if(type==DOT){
        out += ".";
        out += value;
      }
      return out;
    }

    virtual void print(std::ostream &dst) const override
    {
      std::string out = postfix_expr->evaluate();
      if(type==DOT){
        out += ".";
        out += value;
      }
      dst << out;
    }

    virtual void mips_address(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if(type==DOT){
        std::string addr = context.member_to_addr(value);
        postfix_expr->mips_address(dst, context, destReg);
        dst << "addiu " << destReg << "," << destReg << "," << addr << std::endl;
      }else{
        postfix_expr->mips_address(dst, context, destReg);
      }
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      if(type==DOT){
        std::string addr = context.member_to_addr(value);
        if(context.write(postfix_expr->get_id())){
          std::string freeReg = context.alloc_reg(INT);
          postfix_expr->mips_address(dst, context, freeReg);
          dst << "addiu " << freeReg << "," << freeReg << "," << addr << std::endl;
          dst << "sw " << destReg << "," << ",0(" << freeReg << ")" << std::endl;
          context.dealloc_reg(freeReg);
        }else{
          postfix_expr->mips_address(dst, context, destReg);
          dst << "addiu " << destReg << "," << destReg << "," << addr << std::endl;
          dst << "lw " << destReg << "," << ",0(" << destReg << ")" << std::endl;
        }
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

class UnaryExpression : public Expression
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

class Operator : public Expression
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

class MultiplicativeExpression : public Operator
{
  protected:
    ExpressionEnum type;
	public:
		MultiplicativeExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left,_right)
    , type(_type) {}

    ~MultiplicativeExpression(){
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

class AdditiveExpression : public Operator
{
  protected:
    ExpressionEnum type;
	public:
		AdditiveExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~AdditiveExpression(){
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

class ShiftExpression : public Operator
{
  protected:
    ExpressionEnum type;
	public:
		ShiftExpression(ExpressionEnum _type, Expression* _left, Expression* _right)
    : Operator(_left, _right)
    , type(_type) {}

    ~ShiftExpression(){
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
      if(context.get_type(postfix_expr->get_id())==CHAR){
        dst << "move " << freeReg << ",$0" << std::endl;
        dst << "li " << freeReg << ",255" << std::endl;
        dst << "and " << destReg << "," << freeReg << "," << destReg << std::endl;
      }
      context.dealloc_reg(freeReg);
    }
};


/* -------------------------------- Assignment Expression -------------------------------- */

class Assignment_Expression : public Expression // Base class
{
	protected:
		Expression* lvalue;
		Expression* expression;

	public:
		Assignment_Expression(Expression* _lvalue, Expression* _expression) : lvalue(_lvalue), expression(_expression) {}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment(Expression* _lvalue, Expression* _expr) : Assignment_Expression(_lvalue, _expr) {}
};


/* -------------------------------- Relational Binary Expressions -------------------------------- */

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

#endif
