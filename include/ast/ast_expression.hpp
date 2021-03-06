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
};

/* ------------------------------------						     Unary Expression						------------------------------------ */


class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression (Expression* _expression) : expression(_expression) {}
};

/* ------------------------------------						    Post-fix Expression						------------------------------------ */

class Post_Increment_Expression : public Unary_Expression
{
	public:
		Post_Increment_Expression (Expression* _expression) : Unary_Expression(_expression) {}
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
			std::string temp_register = "v1";
			expression->compile(dst, context);

			context.deallocate_stack();

			// Load registers 
			context.load_register(dst, destination_register, frame_pointer_1);
			context.load_register(dst, temp_register, frame_pointer_2);
			context.output_store_operation(dst, operator_type, temp_register, destination_register, 0);

			// Output Direct Assignment operation
			dst << "\t" << "move" << "\t" << "$" << destination_register << ",$" << temp_register << std::endl;

			// Store results
			context.store_register(dst, destination_register, frame_pointer_1);
		}
};


/* ------------------------------------						   Operator Expression						------------------------------------ */

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
			std::string temp_register = "v1";
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
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression (Expression* _left, Expression* _right) : Operator (_left, _right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			dst << "\t" << "subu" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
		}	
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

			dst << "\t" << "multu" << "\t" << "$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "mflo" << "\t" << "$" << destination_register << std::endl;
		}	

};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}

		virtual void execute(std::ostream &dst, Context& context, type type, std::string destination_register, std::string temprorary_register) const override
		{
			// https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division

			dst << "\t" << "divu" << "\t" << "$" << destination_register << ",$" << destination_register << ",$" << temprorary_register << std::endl;
			dst << "\t" << "mfhi" << "\t" << "$" << destination_register << std::endl;
			dst << "\t" << "mflo" << "\t" << "$" << destination_register << std::endl;
		}	
};

/* ------------------------------------					   Relational Operator Expressions				------------------------------------ */

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
			dst << "\t" << "xori " << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x1 << std::endl;
			dst << "\t" << "andi " << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x00ff << std::endl;
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
			dst << "\t" << "xori " << "\t" << "$" << destination_register << ",$" << destination_register << "," << 0x1 << std::endl;
		}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression (Expression* _left, Expression* _right) : Operator (_left,_right) {}
};



#endif