#ifndef EXPRESSIONS
#define EXPRESSIONS

#include "ast_node.hpp"

enum ExpressionEnum { IDENTIFIER, CONSTANT, STRING_LITERAL, DOT, ARROW, INCREMENT, DECREMENT, REFERENCE, DEREFERENCE,
                      PLUS, MINUS, BITWISE_NOT, LOGICAL_NOT, SIZEOF, MULTIPLY, DIVIDE, MODULO, BITWISE_SHIFT_LEFT, BITWISE_SHIFT_RIGHT,
                      LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
                      DIV_ASSIGN, MOD_ASSIGN, SHIFT_LEFT_ASSIGN, SHIFT_RIGHT_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN };


/* -------------------------------- Expression Base Class -------------------------------- */

class Expression
{
public:
    virtual ~Expression()
    {}

    //! Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst) const =0;
};

/* -------------------------------- Primatives -------------------------------- */

class Constant : public Expression
{
	private:
    ExpressionEnum type;

	public:
		Constant(ExpressionEnum _type, int _value)
    : value(_value)
    , type(_type)
    {}

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

		virtual void print_mips(std::ostream &dst, Context &context) const override
		{
			std::string destReg = context.get_dest_reg();
      int val = ;
			dst << "li $" << destReg << "," << val << std::endl;
		}
};

class StringLiteral : public Expression
{
	private:
		std::string value;

	public:
		String_Literal(std::string _string) : value(_string) {}

		virtual void print_structure(std::ostream &dst) const override
		{
			dst << value;
		}

		virtual void print_mips_array(std::ostream &dst, Context &context, std::string destReg, int _addr) const override
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
          dst << "li $" << destReg << "," << ((256*out[0]+out[1])*65536) << std::endl;
          dst << "ori $" << destReg << ",$" << destReg << ",0x" << std::hex << 256*out[2]+out[3] << std::dec << std::endl;
          dst << "sw $" << destReg << "," << addr << "($fp)" << std::endl;
          addr += 4;
          out.clear();
        }
      }

      if(out.size()>=2){
        dst << "li $" << destReg << "," << (256*out[0]+out[1]) << std::endl;
        dst << "sh $" << destReg << "," << addr << "($fp)" << std::endl;
        addr += 2;
      }
      if(out.size()==1){
        dst << "sb $0," << addr << "($fp)" << std::endl;
      }
		}

    virtual void print_mips_pointer(std::ostream &dst, Context &context) const override
		{
      std::string label = context.make_literal_label("$LC", value);
      dst << "lui $" << destReg << ",%hi(" << label << ")" << std::endl;
      dst << "addiu $" << destReg << ",$" << destReg << ",%lo(" << label << ")" << std::endl;
		}
};

class Identifier : public Expression
{
	private:
		std::string value;

	public:
		Identifier(std::string _ID) : value(_ID) {}

		virtual void print(std::ostream &dst) const override
		{
			dst << value;
		}

		virtual void print_mips(std::ostream &dst, Context &context, std::string destReg) const override
		{
      int addr = context.id_to_addr(value);
      dst << "lw $" << destReg << "," << addr << "($fp)" << std::endl;
		}
};

/* -------------------------------- Unary Expression -------------------------------- */

class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression(Expression* _expression) : expression(_expression) {}
};


/* -------------------------------- Post-fix Expression -------------------------------- */

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression(Expression* _expression) : Unary_Expression(_expression) {}
};


class Function_Call_Expression : public Unary_Expression{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression(Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression(_expression), argument_list(_argument_list) {}

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
