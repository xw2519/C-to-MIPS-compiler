#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include <iomanip>
#include <string>

/* -------------------------------- 			      Base class		 			-------------------------------- */

class Primitive : public Expression {};

/* -------------------------------- 				Derivative class		 		-------------------------------- */

class Constant : public Primitive
{
	private:
		int value;

	public:
		Constant (int _value) : value (_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			context.allocate_stack();
			int frame_pointer = context.get_frame_pointer();
			std::string destination_register = "v0";

			dst << "\t" << "li" << "\t" << "$" << "v0" << ", " << value << std::endl;

			context.store_register(dst, destination_register, frame_pointer);	
		}
};

class Identifier : public Primitive // Local variables with constant
{
	private:
		std::string variable_name;

	public:
		Identifier (std::string _variable_name) : variable_name (_variable_name) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{	
			dst << "# Access variable" << std::endl;
			variable compile_variable = context.get_variable(variable_name);
			int destination_address = context.get_frame_pointer();
			std::string destination_register = "v0";

			load_variable_address(dst, context);

			context.load_register(dst, destination_register, destination_address);
			context.output_load_operation(dst, INT, destination_register, destination_register, 0);
			context.store_register(dst, destination_register, destination_address);
		}

		virtual void load_variable_address(std::ostream &dst, Context& context) const
		{
			dst << "# Load variable" << std::endl;
			int destination_address = context.get_frame_pointer();
			std::string destination_register = "v0";
			variable compile_variable = context.get_variable(variable_name);
		
			dst<< "\t" << "addiu" << "\t" << "$" << destination_register << ",$fp," << compile_variable.get_variable_address() << std::endl;

			context.store_register(dst, destination_register, destination_address);

		}

		
};

class StringLiteral : public Primitive
{
	private:
		std::string str;
	public:
		StringLiteral (std::string _str) : str (_str) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			dst << str;
		}
};


#endif