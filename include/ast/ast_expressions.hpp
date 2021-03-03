#ifndef EXPRESSIONS
#define EXPRESSIONS

#include "ast_node.hpp"

enum ExpressionEnum { INT, CHAR, UNSIGNED, FLOAT, DOUBLE, STRING_LITERAL, DOT, ARROW, INCREMENT, DECREMENT, REFERENCE, DEREFERENCE,
                      PLUS, MINUS, BITWISE_NOT, LOGICAL_NOT, SIZEOF, MULTIPLY, DIVIDE, MODULO, BITWISE_SHIFT_LEFT, BITWISE_SHIFT_RIGHT,
                      LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
                      DIV_ASSIGN, MOD_ASSIGN, SHIFT_LEFT_ASSIGN, SHIFT_RIGHT_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN };


/* -------------------------------- Expression Base Class -------------------------------- */

class Expression
{
protected:
  ExpressionEnum type;

public:
    virtual ~Expression()
    {}
    virtual double evaluate() const =0;
    virtual void print(std::ostream &dst) const =0;
    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const =0;
};

/* -------------------------------- Primitives -------------------------------- */

class Constant : public Expression
{
	public:
    virtual double evaluate() const override =0;

		virtual void print(std::ostream &dst) const override =0;

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

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
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

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual double evaluate() const override
    {
      return value;
    }

    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      std::string label = context.make_float_label(value);
      std::string freeReg = context.alloc_reg(INT);
      dst << "lui " << freeReg << ",%hi(" << label << ")" << std::endl;
      if(type==FLOAT){
        dst << "lwc1 " << destReg << ",%lo(" << label << ")(" << freeReg << ")" << std::endl;
      }else{
        std::string destReg2 = destReg;
        destReg2.pop_back();
        destReg2.push_back(std::stoi(destReg.back()) + 1);
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

		virtual void print_mips_array(std::ostream &dst, Context &context, int _addr, std::string destReg) const
		{
      char current;
      int addr = _addr;
      std::vector<unsigned long> out;
      // use lui, ori, sw, sh and sb to store in stack
      for(int i=1; i<value.size(); i++){
        out.push_back(value[i]);
        if(i==value.size()-1){
          out.pop_back();
          out.push_back(0);
        }
        if(out.size()==4){
          dst << "li " << destReg << "," << ((256*out[0]+out[1])*65536) << std::endl;
          dst << "ori " << destReg << "," << destReg << ",0x" << std::hex << 256*out[2]+out[3] << std::dec << std::endl;
          dst << "sw " << destReg << "," << addr << "($fp)" << std::endl;
          addr += 4;
          out.clear();
        }
      }

      if(out.size()>=2){
        dst << "li " << destReg << "," << (256*out[0]+out[1]) << std::endl;
        dst << "sh " << destReg << "," << addr << "($fp)" << std::endl;
        addr += 2;
      }
      if(out.size()==1){
        dst << "sb $0," << addr << "($fp)" << std::endl;
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
		Identifier(std::string &_ID) : value(_ID) {}

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

    virtual std::string evaluate() const
    {
      return value;
    }

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      int addr = context.id_to_addr(value);
      dst << "lw " << destReg << "," << addr << "($fp)" << std::endl;
		}

    virtual void print_mips_float(std::ostream &dst, Context &context, std::string destReg) const
		{
      int addr = context.id_to_addr(value);
      dst << "lwc1 " << destReg << "," << addr << "($fp)" << std::endl;
		}

    virtual void print_mips_double(std::ostream &dst, Context &context, std::string destReg1, std::string destReg2) const
		{
      int addr = context.id_to_addr(value);
      dst << "lwc1 " << destReg1 << "," << (addr+4) << "($fp)" << std::endl;
      dst << "lwc1 " << destReg2 << "," << addr << "($fp)" << std::endl;
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
    , assignment_expr(_assignment_expr)
    {}
    ~ArrayPostfixExpression()
    {
      delete postfix_expr;
      delete assignment_expr;
    }
    virtual std::string evaluate() const
    {
      return postfix_expr->evaluate();
    }
    virtual void print(std::ostream &dst) const override
    {
      dst << postfix_expr->evaluate();
    }
    virtual void print_mips_read(std::ostream &dst, Context &context, std::string destReg) const
    {
      std::string freeReg1 = context.alloc_reg(INT);
      assignment_expr->print_mips(dst, context, freeReg1);

      std::string freeReg2 = context.alloc_reg(INT);
      postfix_expr->print_mips(dst, context, freeReg2);

      dst << "addu " << freeReg1 << "," << freeReg1 << "," << freeReg2 << std::endl;
      dst << "sll " << freeReg1 << "," << freeReg1 << ",2" << std::endl;
      dst << "lw " << destReg << ",0(" << freeReg1 << ")" << std::endl;
      context.dealloc_reg(freeReg1);
      context.dealloc_reg(freeReg2);
    }
    virtual void print_mips_write(std::ostream &dst, Context &context, std::string destReg) const
    {
      std::string freeReg1 = context.alloc_reg(INT);
      assignment_expr->print_mips(dst, context, freeReg1);

      std::string freeReg2 = context.alloc_reg(INT);
      postfix_expr->print_mips(dst, context, freeReg2);

      dst << "addu " << freeReg1 << "," << freeReg1 << "," << freeReg2 << std::endl;
      dst << "sll " << freeReg1 << "," << freeReg1 << ",2" << std::endl;
      dst << "sw " << destReg << ",0(" << freeReg1 << ")" << std::endl;
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
    , arguments(_arguments)
    {}

    ~FunctionPostfixExpression(){
      delete postfix_expr;
      for(int i=0; i<arguments->size(); i++){
        delete arguments->at(i);
      }
      delete arguments;
    }
    virtual std::string evaluate() const
    {
      return postfix_expr->evaluate();
    }
    virtual void print(std::ostream &dst) const override
    {
      dst << postfix_expr->evaluate();
    }
    virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
    {
      std::vector<ExpressionEnum> arg_types = context.get_arg_types(postfix_expr->evaluate());
      std::vector<std::string> freeRegs;
      std::string paramReg = "$";
      int float_param_count = 0;

      if(arguments){
        if(arguments->size()<=4){
          for(int i=0; i<arguments->size(); i++){
            if((arg_types[i] == FLOAT) || (arg_types[i] == DOUBLE)){
              paramReg.push_back('f');
              paramReg.push_back(std::to_string(2*float_param_count));
              arguments->at(i)->print_mips(dst, context, paramReg);
              float_param_count++;
              paramReg.pop_back();
              paramReg.pop_back();
            }else{
              paramReg.push_back(std::to_string(4+i));
              arguments->at(i)->print_mips(dst, context, paramReg);
              paramReg.pop_back();
            }
          }
        }else{
          dst <<
        }
      }

      dst <<
    }
};

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression(Expression* _expression) : Unary_Expression(_expression) {}
};

/* -------------------------------- Unary Expression -------------------------------- */

class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression(Expression* _expression) : expression(_expression) {}
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

/* -------------------------------- Operator Expression -------------------------------- */

class Operator : public Expression
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Operator(Expression* _left, Expression* _right) : left(_left), right(_right) {}
};

/* -------------------------------- Arithmetic Expression -------------------------------- */

class Add_Expression : public Operator
{
	public:
		Add_Expression(Expression* _left, Expression* _right) : Operator(_left, _right) {}

		virtual void print_MIPS(std::ostream &dst, Context& context, int destination_register) const override
		{
			// Find available registers to allocate and reserve it for the operation
			std::vector<int> available_registers = context.list_available_temprorary_registers();
            context.set_unavaiable(available_registers[0]);

			// Release registers used in this operation
			context.set_avaiable(available_registers[0]);
            context.set_avaiable(available_registers[1]);
		}
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}

		virtual void print_MIPS(std::ostream &dst, Context& context, int destination_register) const override
		{
            // Find available registers to allocate and reserve it for the operation
			std::vector<int> available_registers = context.list_available_temprorary_registers();
            context.set_unavaiable(available_registers[0]);

			// Compiler left and right branches
            left->print_MIPS(dst, context, destination_register);
            right->print_MIPS(dst, context, available_registers[0]);

			// Print MIPS assembly
            dst << "\t"<< "sub"<< "\t" << "$" << destination_register << ", $" << destination_register << ", $" << available_registers[0] << "\t#Subtraction"<< std::endl;

            // Release registers used in this operation
			context.set_avaiable(available_registers[0]);
		}
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression(Expression* _left, Expression* _right) : Operator(_left,_right) {}
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
