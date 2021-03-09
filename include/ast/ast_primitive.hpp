#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include <iomanip>
#include <string>

/* -------------------------------- 			      Base class		 			-------------------------------- */

class Primitive : public Expression{};

/* -------------------------------- 				Derivative class		 		-------------------------------- */

class Constant : public Primitive
{
	private:
		int value;

	public:
		Constant (int _value) : value (_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int frame_pointer = context.get_frame_pointer();
			std::string destination_register = "v0";


			dst << "\t" << "li" << "\t" << "\t" << "$" << "v0" << ", " << value << std::endl;

			context.store_register(dst, destination_register, frame_pointer);	
		}

		virtual int evaluate() const override { return value; };
};

class Identifier : public Primitive // Local variables with constant
{
	private:
		std::string variable_name;

	public:
		Identifier (std::string _variable_name) : variable_name (_variable_name) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{	
			int destination_address = context.get_frame_pointer();

			dst << std::endl;
			dst << "\t" << "# Access variable" << std::endl;
			
			std::string destination_register = "v0";
			//dst << "1" << destination_address << std::endl;

			load_variable_address(dst, context);

			variable compile_variable = context.get_variable(variable_name);

			// Array already dealt with address 
			if(compile_variable.get_declaration_type() != ARRAY)
			{
				// dst << "Trigger" << std::endl;
				context.load_register(dst, destination_register, destination_address);
				context.output_load_operation(dst, INT, destination_register, destination_register, 0);
				context.store_register(dst, destination_register, destination_address);
			}
		}

		virtual void load_variable_address(std::ostream &dst, Context& context) const override
		{
			dst << std::endl;
			dst << "\t" << "# Load variable" << std::endl;
			
			int destination_address = context.get_frame_pointer();
			std::string destination_register = "v0";
			variable compile_variable = context.get_variable(variable_name);

			// std::cerr << variable_name << std::endl;
			// std::cerr << compile_variable.get_variable_address() << std::endl;
		
			dst << "\t" << "addiu" << "\t" << "$" << destination_register << ",$fp," << compile_variable.get_variable_address() << std::endl;

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