#ifndef AST_PRIMITIVES_HPP
#define AST_PRIMITIVES_HPP

#include <iomanip>
#include <string>

/* -------------------------------- 			       Base class		 			-------------------------------- */

class Primitive : public Expression {};

/* -------------------------------- 				Derivative class		 		-------------------------------- */

class Integer : public Primitive
{
	private:
		int value;

	public:
		Integer (int _value) : value(_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$2";

			dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << value << std::endl;

			context.store_register(dst, destination_register, stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(INT); };

		virtual double evaluate() const override { return value; };
};

// Converts to unsigend through casting
class Unsigned_Integer : public Primitive
{
	private:
		unsigned int value;

	public:
		Unsigned_Integer (int _value) : value(_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$2";

			dst << "\t" << "li" << "\t" << "\t" << "$2" << ", " << value << std::endl;

			context.store_register(dst, destination_register, stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(UNSIGNED_INT); };

		virtual double evaluate() const override { return value; };
};

class Float : public Primitive
{
	private:
		float value;

	public: 
		Float (float _value) : value(_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$f0";

			dst << "\t" << "li.s" << "\t" << "\t" << destination_register << ", " << value << std::endl;

			context.store_float_register(dst, destination_register, stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(FLOAT); };

		virtual double evaluate() const override { return value; };
};

class Double : public Primitive
{
	private:
		double value;

	public: 
		Double (float _value) : value(_value) {}

		virtual void compile(std::ostream &dst, Context& context) const override
		{
			int stack_pointer = context.get_stack_pointer();
			std::string destination_register = "$f0";

			dst << "\t" << "li.d" << "\t" << "\t" << destination_register << ", " << value << std::endl;

			context.store_float_register(dst, destination_register, stack_pointer);	
		}

		virtual type get_data_type(Context& context) const override { return type(DOUBLE); };

		virtual double evaluate() const override { return value; };
};

class Identifier : public Primitive
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

			if (compile_variable.get_variable_scope() == GLOBAL)
			{
				dst << "\t" << "la" << "\t" << "\t" << destination_register << "," << variable_name << std::endl;
			}
			else 
			{
				dst << "\t" << "addiu" << "\t" << destination_register << ",$30," << compile_variable.get_variable_address() << std::endl;
			}

			context.store_register(dst, destination_register, destination_address);
		}

		virtual type get_data_type(Context& context) const { return context.get_variable(variable_name).get_variable_type(); };

		virtual type_definition get_type_definition(Context& context) const override { return context.get_variable(variable_name).get_type_definition(); };

		virtual bool get_pointer_capability(Context& context) const { return context.get_variable(variable_name).get_pointer_capability(); };
};

// https://stackoverflow.com/questions/42183471/declaring-a-pointer-in-mips


#endif