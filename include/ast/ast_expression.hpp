#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include <string>
#include <iostream>
#include <map>
#include <memory>

class Expression;

typedef const Expression *Expression_Ptr;


/* ------------------------------------						   Expression Base Class					------------------------------------ */
class Expression : public Node 
{
	public:
		// Primitive functions
		virtual void load_variable_address(std::ostream &dst, Context& context) const {};

		// Evaluate the expression
		virtual int evaluate() const { return 0; };
};

/* ------------------------------------						     Unary Expression						------------------------------------ */

class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression (Expression* _expression) : expression(_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "Triggered 1" << std::endl;
		}
};

/* ------------------------------------						    Post-fix Expression						------------------------------------ */

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression (Expression* _expression) : Unary_Expression(_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "Triggered 2" << std::endl;
		}
};


class Function_Call_Expression : public Unary_Expression
{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression (Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression (_expression), argument_list (_argument_list) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "Triggered 3" << std::endl;
		}

};

/* ------------------------------------						  Assignment Expressions					------------------------------------ */

class Assignment_Expression : public Expression // Base class
{
	protected:
		Expression* left_value;
		Expression* expression;
			
	public:
		Assignment_Expression (Expression* _left_value, Expression* _expression) : left_value (_left_value), expression (_expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << "Triggered 4" << std::endl;
		}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment (Expression* _left_value, Expression* _expression) : Assignment_Expression (_left_value, _expression) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			type operator_type = INT; // For now

			// Destination register set to v0 by default
			int frame_pointer_1 = context.get_frame_pointer();

			std::string destination_register = "v0";
			left_value->load_variable_address(dst, context);		

			// Create temprorary register to handle expression
			context.allocate_stack();

			int frame_pointer_2 = context.get_frame_pointer(); 
			std::string temp_register_1 = "t0";
			expression->compile(dst, context);

			context.deallocate_stack();

			// Load registers 
			context.load_register(dst, destination_register, frame_pointer_1);
			context.load_register(dst, temp_register_1, frame_pointer_2);
			context.output_store_operation(dst, operator_type, temp_register_1, destination_register, 0);

			// Output Direct Assignment operation
			dst << "\t" << "move" << "\t" << "$" << destination_register << ",$" << temp_register_1 << std::endl;

			// Store results
			context.store_register(dst, destination_register, frame_pointer_1);
		}
};

/* ------------------------------------						    Operator Expression						------------------------------------ */

class Operator : public Expression
{
	protected:
		Expression* left;
		Expression* right;

	public:
		Operator (Expression* _left, Expression* _right) : left (_left), right (_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Only supports INT for now
			type operator_type = INT;

			// Select registers and allocate frame pointers
			int frame_pointer_1 = context.get_frame_pointer(); // Find current frame pointer 

			std::string destination_register = "v0";
			left->compile(dst, context);

			context.allocate_stack(); // Add new temprorary register

			int frame_pointer_2 = context.get_frame_pointer();
			std::string temp_register = "t0";
			right->compile(dst, context);

			context.deallocate_stack(); // Reduce frame pointer as temproray register done

			// Load registers 
			context.load_register(dst, destination_register, frame_pointer_1);
			context.load_register(dst, temp_register, frame_pointer_2);

			// Execute
			execute(dst, context, operator_type, destination_register, temp_register);

			// Store result
			context.store_register(dst, destination_register, frame_pointer_1);
		}
};

/* ------------------------------------						   Arithmetic Expression					------------------------------------ */

class Add_Expression : public Operator
{
	public:
		Add_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			dst << "\t" << "addu" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}

		virtual int evaluate() const override { return left->evaluate() + right->evaluate(); };
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			dst << "\t" << "subu" << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	

		virtual int evaluate() const override { return left->evaluate() - right->evaluate(); };
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

			dst << "\t" << "multu" << "\t" << "\t"  << "$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "mflo" << "\t" << "\t"  << "$" << destination_register << std::endl;
		}	

		virtual int evaluate() const override { return left->evaluate() * right->evaluate(); };
};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

			dst << "\t" << "divu" << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "mfhi" << "\t" << "\t" << "$" << destination_register << std::endl;
			dst << "\t" << "mflo" << "\t" << "\t" << "$" << destination_register << std::endl;
		}	

		virtual int evaluate() const override { return left->evaluate() / right->evaluate(); };
};

/* ------------------------------------					    Relational Operator Expressions				------------------------------------ */

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}
	
		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt " << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt " << "\t" << "$" << destination_register << ",$" << temprorary_register << ",$" << destination_register << std::endl;
		}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt "  << "\t" << "$" << destination_register << ",$" << temprorary_register << ",$" << destination_register << std::endl;
			dst << "\t" << "xori " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x1 << std::endl;
			dst << "\t" << "andi " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x00ff << std::endl;
		}	
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "slt "  << "\t" << "$" << destination_register << ",$" << temprorary_register << ",$" << destination_register << std::endl;
			dst << "\t" << "xori " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x1 << std::endl;
		}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{

			dst << "\t" << "xor " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "sltiu " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << "," << 1 << std::endl;
		}	
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			
			dst << "\t" << "xor " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "sltu " << "\t" << "\t" << "$" << destination_register << ",$0" << ",$" << destination_register << std::endl;
		}	
};

/* ------------------------------------					     Bitwise Operator Expressions				------------------------------------ */

class Bitwise_AND_Expression : public Operator
{
	public:
		Bitwise_AND_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "and " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	
};

class Bitwise_OR_Expression : public Operator
{
	public:
		Bitwise_OR_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "or " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	
};

class Bitwise_XOR_Expression : public Operator
{
	public:
		Bitwise_XOR_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf

			dst << "\t" << "xor " << "\t" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	
};

/* ------------------------------------					     Logical Operator Expressions				------------------------------------ */

class Logical_AND_Expression : public Operator
{
	public:
		Logical_AND_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Establish jump variables
			context.make_label("LOGICAL_AND");
			std::string return_label = context.get_function_return_label();

			std::string destination_name = "v0";
			int destination_address = context.get_frame_pointer();

			left->compile(dst, context);
			context.load_register(dst, destination_name, destination_address);

			dst << "\t" << "beq " << "\t" << "$" << destination_register << ",$" << 0 << "," << return_label << std::endl;
			
			context.allocate_stack();
			int frame_pointer_temp_reg = context.get_frame_pointer();

			std::string temp_register_1 = "v1";
			right->compile(dst, context);

			context.load_register(dst, temp_register_1, frame_pointer_temp_reg);
			context.deallocate_stack();

			dst << "\t" << "move " << "\t" << "$" << destination_register << ",$" << 0 << std::endl;
			dst << "\t" << "beq " << "\t" << "$" << temp_register_1 << ",$" << 0 << "," << return_label << std::endl;

			dst << "\t" << "addiu " << "\t" << "\t" << "$" << destination_register << ",$" << 0 << "," << 1 << std::endl;

			context.store_register(dst, destination_name, destination_address);
		}

};

class Logical_OR_Expression : public Operator
{
	public:
		Logical_OR_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// Establish jump variables
			std::string return_label_1 = context.make_label("LOGICAL_OR");
			std::string return_label_2 = context.make_label("LOGICAL_OR");

			std::string destination_name = "v0";
			int destination_address = context.get_frame_pointer();

			left->compile(dst, context);
			context.load_register(dst, destination_name, destination_address);

			dst << "\t" << "bne " << "\t" << "\t" << "$" << destination_register << ",$" << 0 << "," << return_label_1 << std::endl;
			
			context.allocate_stack();
			int frame_pointer_temp_reg = context.get_frame_pointer();

			std::string temp_register_1 = "v1";
			right->compile(dst, context);

			context.load_register(dst, temp_register_1, frame_pointer_temp_reg);
			context.deallocate_stack();

			dst << "\t" << "move " << "\t" << "$" << destination_register << ",$" << 0 << std::endl;
			dst << "\t" << "beq " << "\t" << "$" << temp_register_1 << ",$" << 0 << "," << return_label_2 << std::endl;

			dst << "\t" << "addiu " << "\t" << "$" << destination_register << ",$" << 0 << "," << 1 << std::endl;


			dst << "\t" << return_label_1 << ":" << std::endl; 
			dst << "\t" << "addiu " << "\t" << "$" << destination_register << ",$" << 0 << "," << 1 << std::endl;
			dst << "\t" << return_label_2 << ":" << std::endl; 

			context.store_register(dst, destination_name, destination_address);
		}
};



#endif