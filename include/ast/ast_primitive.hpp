#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include <iomanip>
#include <string>

/* -------------------------------- 			      Base class		 			-------------------------------- */

class Primitive : public Expression{};

/* -------------------------------- 				Derivative class		 		-------------------------------- */

class Integer : public Primitive
{
	private:
		int value;

	public:
		Integer (int _value) : value (_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$2";

			dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << value << std::endl;

			context.store_register(dst, destination_register, stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(INT); };

		virtual int evaluate() const override { return value; };
};

class Float : public Primitive
{
	private:
		float value;

	public: 
		Float (std::string _value) : value (stof(_value)) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Generate float label
			std::string float_label = context.make_label("FLOAT");

			int stack_pointer = context.get_stack_pointer();
			std::string temp_register = "$8";

			dst << "\t" << "la" << "\t" << "\t" << temp_register << ", " << float_label << std::endl;
			context.output_load_operation(dst, FLOAT, "$2", temp_register, 0);
			context.store_register(dst, "$2", stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(FLOAT); };

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
			int destination_address = context.get_stack_pointer();

			dst << "\t" << "# Access variable" << std::endl;
			
			std::string destination_register = "$2";

			load_variable_address(dst, context);

			variable compile_variable = context.get_variable(variable_name);

			if(compile_variable.get_declaration_type() != ARRAY)
			{
				context.output_load_operation(dst, INT, destination_register, destination_register, 0);
				context.store_register(dst, destination_register, destination_address);
			}
		}

		virtual std::string get_variable_name() const override { return variable_name; }; 

		virtual void load_variable_address(std::ostream &dst, Context& context) const override
		{
			dst << "\t" << "# Load variable" << std::endl;

			int destination_address = context.get_stack_pointer();
			std::string destination_register = "$2";
			variable compile_variable = context.get_variable(variable_name);

			// std::cerr << variable_name << std::endl;
			// std::cerr << compile_variable.get_variable_address() << std::endl;
		
			dst << "\t" << "addiu" << "\t" << destination_register << ",$30," << compile_variable.get_variable_address() << std::endl;

			context.store_register(dst, destination_register, destination_address);
		}

		virtual type get_data_type(Context& context) const { return context.get_variable(variable_name).get_variable_type(); };
};

// https://stackoverflow.com/questions/42183471/declaring-a-pointer-in-mips

class Pointer_literal : public Primitive
{
	private:
		std::string pointer_name;
	public:
		Pointer_literal(std::string _pointer_name) : pointer_name(_pointer_name) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			// Get storage parameters
			std::string pointer_register = "$2";
			int pointer_address = context.get_stack_pointer();
			
			// Generate pointer jump label
			std::string pointer_label = context.make_label("POINTER");

			// Add into tracker
			context.add_label_variable(pointer_label, pointer_name);
			context.add_label_declaration(pointer_name, "asciiz");

			dst << "\t" << "la" << "\t" << "\t" << pointer_register << "," << pointer_label << std::endl;

			// Store pointer
			context.store_register(dst, pointer_register, pointer_address);
		}
};


#endif