#ifndef CONTEXT
#define CONTEXT

#include "ast_node.hpp"


/*
$0	        $zero	    Hard-wired to 0 										(Not used for normal use)
$1	        $at	        Reserved for pseudo-instructions						(Not used for normal use)
$2  - $3	$v0,  $v1	Return values from functions							(Not used for normal use)
$4  - $7	$a0 - $a3	Procedure arguments - not preserved by subprograms		(Not used for normal use)
$8  - $15	$t0 - $t7	Temporary registers, not preserved by subprograms
$16 - $23	$s0 - $s7	Saved registers, preserved by subprograms
$24 - $25	$t8 - $t9	More temporary registers, not preserved by subprograms
$26 - $27	$k0 - $k1	Reserved for kernel. Do not use.						(Not used for normal use)
$28	        $gp	        Global Area Pointer (base of global data segment)		(Not used for normal use)
$29	        $sp	        Stack Pointer											(Not used for normal use)
$30	        $fp	        Frame Pointer											(Not used for normal use)
$31	        $ra	        Return Address											(Not used for normal use)
------------------------------------------------------------------------------------------------------------
$4  - $7				Procedure arguments
$8 	- $15				Temporary registers that can be overwritten by called procedures
$16 - $23				Temporary registers but cannot be overwritten by procedures
$24	- $25				Temporary registers
*/

/* ------------------------------------ 			  				Typedef variables		 					------------------------------------ */

typedef std::map<std::string, std::string> type_store;
typedef std::map<std::string, std::vector<std::string>> function_store;

/* ------------------------------------									Context Functions						------------------------------------ */
class Context
{
	private:
		std::vector<type_store> type_scopes;               // stores identifier and associated type
		std::vector<type_store> other_scopes;              // stores identifier and whether it is a function, array, struct or enum
		std::vector<function_store> function_types;        // stores function with return type and param types
		std::vector<type_store> typedef_types;             // stores typeIDs followed by types
		std::vector<std::string> id_locations;

		// Trackers and counters
		int label_count = 0;
		std::vector<bool> int_regs;
		std::vector<bool> float_regs;
		bool global = true;
		std::string current_function = "global";

		std::map<std::string, double> float_labels;
		std::map<std::string, std::string> string_labels;

	public:
		Context()
		{
			for(int i=0; i<10; i++){
				int_regs.push_back(false);
				float_regs.push_back(false);
			}

			type_scopes.push_back(type_store());
			other_scopes.push_back(type_store());
			function_types.push_back(function_store());
			typedef_types.push_back(type_store());
		}
		/* ------------------------------------						    Label Functions						------------------------------------ */

		std::string make_label()
		{
			std::string label = "$LC";
			label += std::to_string(label_count);
			label_count++;
			return label;
		}

		std::string make_float_label(double value)                                   // get label of  ".word <float value> directive"
		{
			std::string label = "$LC";
			label += std::to_string(label_count);
			label_count++;
			float_labels[label] = value;
			return label;
		}

    std::string make_string_label(std::string value)                             // get label of  ".ascii <string literal/000> directive"
		{
			std::string label = "$LC";
			label += std::to_string(label_count);
			label_count++;
			string_labels[label] = value;
			return label;
		}

    /* ------------------------------------				  Register Allocation				  ------------------------------------ */

    std::string next_reg(std::string someReg)                                   // return name of next register
		{
			int next;
			if(someReg[1]=='f'){
				if(someReg.size()==3){
					next = someReg.back()+1;
					someReg.pop_back();
				}else{
					next = someReg.back()+11;
					someReg.pop_back();
					someReg.pop_back();
				}
			}else{     // 8-15, 24, 25
				if(someReg.size()==2){
					next = someReg.back()+1;
					someReg.pop_back();
				}else{
					next = someReg.back();
					someReg.pop_back();
					next = next + 10*someReg.back();
					someReg.pop_back();
					if(next==16){ next = 24; }
					else{ next++; }
				}
			}
			someReg += std::to_string(next);
			return someReg;
		}

    std::string alloc_reg(ExpressionEnum type, int amount=1)                    // return name of free register, mark it and following amount as occupied
		{
			bool found = false;
			int begin, consec = 0;
			std::string foundReg = "$";

			if((type==FLOAT) || (type==DOUBLE)){
				foundReg += 'f';
				for(int i=0; i<16; i++){
					if(!float_regs[i]){
						if(!consec){ begin = i; }
						consec++;
						if(consec==amount){ break; }
					}else{
						consec = 0;
					}
				}
				for(int i = begin; i<begin+amount; i++){
					float_regs[i] = true;
				}
			}else{
				for(int i=0; i<10; i++){
					if(!int_regs[i]){
						if(!consec){ begin = i; }
						consec++;
						if(consec==amount){ break; }
					}else{
						consec = 0;
					}
				}
				for(int i = begin; i<begin+amount; i++){
					int_regs[i] = true;
				}
				if(begin<8){ begin += 8; }
				else{ begin += 16; }
			}
			foundReg += std::to_string(begin);
			return foundReg;
		}

    void dealloc_reg(std::string someReg, int amount=1)                         // free register and following amount of registers
		{
			int begin = someReg.back();
			someReg.pop_back();
			if((someReg.size() - (someReg[1]=='f')) > 1){
				begin = begin + 10*someReg.back();
			}
			if(someReg[1]!='f'){
				if(begin>15){ begin -= 16; }
				else{ begin -= 8; }
			}
			for(int i=begin; i<begin+amount; i++){
				if(someReg[1]=='f'){ float_regs[i] = false; }
				else{ int_regs[i] = false; }
			}
		}

    /* ------------------------------------				  Getting information				  ------------------------------------ */

		void add_identifier(std::string ident)
		{
			if((type_scopes.back()).find(ident)==(type_scopes.back()).end()){
				type_scopes.back()[ident] = "unknown";
			}
			for(int i=0; i<id_locations.size(); i++){
				if(id_locations[i]==ident){
					return;
				}
			}
			id_locations.push_back(ident);
		}

		void add_variable(std::string type, std::string ident)
		{
			type_scopes.back()[ident] = type;
			for(int i=0; i<id_locations.size(); i++){
				if(id_locations[i]==ident){
					return;
				}
			}
			id_locations.push_back(ident);
		}

		void initialize(std::string ident, Expression* expr)
		{}

		void add_enum(std::string ident, std::vector<Enumerator*>* enum_declrs)
		{
			type_scopes.back()[ident] = "enumtype";
		}

		void add_struct(std::string ident, std::vector<StructDeclaration*>* member_declrs)
		{
			type_scopes.back()[ident] = "structtype";
		}

		void add_function(std::string type, std::string ident, Statement* statements)
		{
			std::vector<std::string> types;
			types.push_back(type);
			type_scopes.back()[ident] = type;
			other_scopes.back()[ident] = "function";
			function_types.back()[ident] = types;
			current_function = ident;
		}

		void declare_function(std::string ident)
		{}

		void add_array(std::string ident, int size)
		{}

		void add_typedef(std::string base_type, std::string type_id)
		{}

    /* ------------------------------------				  Code generation				  ------------------------------------ */

    bool check_global(std::string identifier)                                   // return true if identifier is global
		{
			return global;
		}

		void new_scope()
		{
			global = false;
			type_scopes.push_back(type_scopes.back());
			other_scopes.push_back(other_scopes.back());
			function_types.push_back(function_types.back());
			typedef_types.push_back(typedef_types.back());
		}

		void old_scope()
		{
			type_scopes.pop_back();
			other_scopes.pop_back();
			function_types.pop_back();
			typedef_types.pop_back();
			if(type_scopes.size()==1){ global = true; }
		}

    ExpressionEnum get_type(std::string identifier)                             // return type of identifier
		{
			std::string str_type = type_scopes.back()[identifier];
			if(str_type==std::string("int")){ return INT; }
		}

    ExpressionEnum get_type_pointed(std::string identifier)                     // return type pointed to by identifier
		{
			return INT;
		}

    ExpressionEnum get_type_member(std::string identifier, std::string member)  // return type of member of struct
		{
			return INT;
		}

    std::string id_to_addr(std::string ident)                              // return address, relative to $fp, of identifier
		{
			for(int i=0; i<id_locations.size(); i++){
				if(id_locations[i]==ident){
					return std::to_string(4*(id_locations.size()-i)+8);
				}
			}
		}

    std::string member_to_addr(std::string identifier, std::string member)      // return address of member, relative to beginning of struct
		{
			return "not implemented yet";
		}

    int size_of_pointed(std::string identifier)                                 // return size of value pointed to by identifier, in words
		{
			return 1;
		}

    int size_of_member(std::string identifier, std::string member)              // return size of member, in words
		{
			return 1;
		}

    int size_of(std::string identifier)                                         // return size of identifier in words
		{
			return 1;
		}

    int size_of(Declaration* declr)                                             // return size of declaration
		{
			return 1;
		}

		std::string typeid_to_type(std::string typeident)
		{
			return typedef_types.back()[typeident];
		}

		int get_stack_size(std::string ident)
		{
			//return 4*id_locations.size() + 16;
			return 120;
		}

		int get_stack_size_current()
		{
			//return 4*id_locations.size() + 16;
			return 120;
		}
};


#endif
