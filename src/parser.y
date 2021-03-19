/*

Contents and terms are mostly derived from the ANSI C grammer specification (ANSI/ISO 9899/1990) specification

https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#constant-expressions

*/

%define parse.error verbose 

%code requires
{
	#include <cassert>
	#include <vector>

	#include "ast.hpp"

	extern const Node *root; 

	int yylex(void);
	void yyerror(const char *);
}

%union
{
	const Node 		*node;
	Declarator 		*declarator_node;
	Declaration 	*declaration_node;
	Statement 		*statement_node;
	Expression 		*expression_node;
	std::string 	*string;
	int 			int_num;
	unsigned int 	unsigned_num;
	float			float_num;
	double 			double_num;
	type_definition *type_node;

	std::vector<Expression*>* 	argument_list_vector;
	std::vector<Statement*>* 	statement_list_vector;
	std::vector<Declaration*>* 	declaration_list_vector;
	std::vector<Declarator*>* 	declarator_list_vector;

	std::vector<Expression*>* 	initialisation_list;
}

/* ------------------------------------					Tokens					------------------------------------ */

// Logical Operators
%token T_LOGICAL_AND T_LOGICAL_OR T_RIGHT_SHIFT T_LEFT_SHIFT
// Assignment Operators
%token T_ASSIGN T_INCREMENT T_DECREMENT T_ADD_ASSIGN T_SUB_ASSIGN
// Arithmetic Operators
%token T_MULTIPLY T_DIVIDE T_PLUS T_MINUS T_MODULO
// Characters Operators
%token T_LBRACKET T_RBRACKET T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET T_COLON T_SEMICOLON T_COMMA
// Comparison Operators
%token T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL T_EQUAL T_NOT_EQUAL
// Types Operators
%token T_INT T_VOID T_CHAR T_DOUBLE T_FLOAT T_UNSIGNED
// Sizeof
%token T_SIZEOF
// Structures Operators
%token T_IF T_ELSE T_SWITCH T_WHILE T_FOR T_CONTINUE T_BREAK T_RETURN T_DEFAULT T_CASE
// Rules
%token T_IDENTIFIER T_INT_CONSTANT T_FLOAT_CONSTANT T_DOUBLE_CONSTANT T_UNSIGNED_CONSTANT
// Bitwise
%token T_BITWISE_AND T_BITWISE_OR T_BITWISE_XOR
// Enumeration
%token T_ENUM


/* ------------------------------------					Type					------------------------------------ */

%type <node> program 
%type <node> function_definition
%type <node> global_declaration
%type <node> enum_definition

// Variables
%type <declarator_node>	declarator initialisation_declarator
%type <declarator_list_vector> initialisation_declarator_list
%type <declaration_node> declaration parameter_declaration 
%type <declaration_list_vector> declaration_list parameter_list

%type <initialisation_list> initialisation_list
%type <argument_list_vector> argument_list

// Enum
%type <declarator_node> enumerator_declarator enumerator_initialisation
%type <declarator_list_vector> enumerator_list
%type <declaration_node> enumerator

// Expressions
%type <expression_node> primary_expression unary_expression postfix_expression 
%type <expression_node> multiply_expression add_expression 
%type <expression_node> bitwise_AND_expression bitwise_XOR_expression bitwise_OR_expression
%type <expression_node> logical_AND_expression logical_OR_expression
%type <expression_node> cast_expression
%type <expression_node> bitwise_shift_expression compare_expression equal_expression
%type <expression_node> logical_expression ternary_expression 
%type <expression_node> assignment_expression expression 

// Statements
%type <statement_node> statement 
%type <statement_list_vector> statement_list
%type <statement_node> jump_statement compound_statement expression_statement condition_statement iteration_statement labeled_statement

// Types
%type <type_node> TYPE 
%type <string> 	T_IDENTIFIER T_INT T_RETURN T_FLOAT T_UNSIGNED

%type <int_num> 	 T_INT_CONSTANT
%type <float_num> 	 T_FLOAT_CONSTANT
%type <double_num> 	 T_DOUBLE_CONSTANT
%type <unsigned_num> T_UNSIGNED_CONSTANT

%start ROOT

%%

/* ------------------------------------							 Base definitions							------------------------------------ */

ROOT 							: 	program																
									{ root = $1; }

program 						: 	global_declaration					  								
									{ $$ = $1; }

								| 	program global_declaration   										
									{ $$ = new Program($1,$2); }

global_declaration				:	function_definition													
									{ $$ = $1; }

								|	declaration 														
									{ $$ = $1; }

								|	enumerator
									{ $$ = $1; }

function_definition				:	TYPE T_IDENTIFIER T_LBRACKET parameter_list T_RBRACKET compound_statement 
									{ $$ = new Function_Definition(*$1, *$2, $4, $6); } 

initialisation_list 			:   assignment_expression 												
									{ $$ = new std::vector<Expression*>(1, $1);	}

								|	initialisation_list T_COMMA assignment_expression 					
									{ $1->push_back($3); $$=$1; }

declaration 					:	TYPE T_SEMICOLON													
									{ $$ = new Declaration(*$1); }

								| 	TYPE initialisation_declarator_list T_SEMICOLON 					
									{ $$ = new Declaration(*$1, $2); }

declaration_list				: 	declaration 														
									{ $$ = new std::vector<Declaration*>(1, $1); } 

								| 	declaration_list declaration  										
									{ $1->push_back($2); $$ = $1; }

parameter_declaration			:	TYPE declarator 													
									{ $$ = new Declaration(*$1, new std::vector<Declarator*>(1, $2)); }	
								

parameter_list					:	parameter_declaration				     							
									{ $$ = new std::vector<Declaration*>(1, $1); }

								|	parameter_list T_COMMA parameter_declaration 						
									{ $1->push_back($3); $$ = $1; }

								| 	{ $$ = NULL; }

declarator						: 	T_IDENTIFIER 														
									{ $$ = new Variable_Declarator(*$1); }

								| 	T_IDENTIFIER T_SQUARE_LBRACKET T_SQUARE_RBRACKET					
									{ $$ = new Array_Declarator(*$1, NULL); }

								| 	T_IDENTIFIER T_SQUARE_LBRACKET expression T_SQUARE_RBRACKET			
									{ $$ = new Array_Declarator(*$1, $3); }

								|	T_IDENTIFIER T_LBRACKET parameter_list T_RBRACKET
									{ $$ = new Function_Prototype_Declaration(*$1, $3); }

initialisation_declarator		: 	declarator 															
									{ $$ = $1; }

								| 	declarator T_ASSIGN assignment_expression 							
									{ $$ = new Initialisation_Variable_Declarator($1, $3); }

								| 	declarator T_ASSIGN T_CURLY_LBRACKET initialisation_list T_CURLY_RBRACKET 							
									{ $$ = new Initialisation_Array_Declarator($1, $4); }

								| 	declarator T_ASSIGN T_CURLY_LBRACKET T_CURLY_RBRACKET				
									{ $$ = new Initialisation_Array_Declarator($1, new std::vector<Expression*>); }

initialisation_declarator_list	: 	initialisation_declarator 											
									{ $$ = new std::vector<Declarator*>(1, $1);	}

								|	initialisation_declarator_list T_COMMA initialisation_declarator	
									{ $1->push_back($3); $$ = $1; }


/* ------------------------------------							   Enumerator									------------------------------------ */

enumerator						:	TYPE T_IDENTIFIER T_CURLY_LBRACKET enumerator_list T_CURLY_RBRACKET T_SEMICOLON
									{ $$ = new Declaration(*$1, $4); } 

enumerator_list					: 	enumerator_initialisation
									{ $$ = new std::vector<Declarator*>(1, $1); }

								| 	enumerator_list T_COMMA enumerator_initialisation
									{ $1->push_back($3); $$ = $1; }

enumerator_initialisation		: 	enumerator_declarator T_ASSIGN assignment_expression
									{ $$ = new Initialisation_Enum_Declarator($1, $3); }
								
								|	enumerator_declarator
									{ $$ = new Initialisation_Enum_Declarator($1); }

enumerator_declarator			: 	T_IDENTIFIER
									{ $$ = new Variable_Declarator(*$1); }


/* ------------------------------------							 Expression Base								------------------------------------ */

expression 						:	assignment_expression
									
primary_expression				: 	T_INT_CONSTANT															
									{ $$ = new Integer($1); }

								|	T_FLOAT_CONSTANT
									{ $$ = new Float($1); }
								
								|	T_UNSIGNED_CONSTANT
									{ $$ = new Unsigned_Integer($1); }

								| 	T_IDENTIFIER		 												
									{ $$ = new Identifier(*$1);	}	
								
								| 	T_LBRACKET expression T_RBRACKET									
									{ $$ = $2; }		
								
								|	T_SIZEOF T_LBRACKET TYPE T_RBRACKET
									{ $$ = new Sizeof_Type_Expression(*$3); }	
								
								|	T_SIZEOF T_LBRACKET T_IDENTIFIER T_RBRACKET
									{ $$ = new Sizeof_Variable_Expression(*$3); }

unary_expression				: 	postfix_expression

								|	T_BITWISE_AND cast_expression
									{ $$ = new Reference_Expression($2); }
								
								|	T_MULTIPLY cast_expression
									{ $$ = new Dereference_Expression($2); }

								|	T_MINUS cast_expression
									{ $$ = new Pre_Negative_Expression($2); }

								|	T_INCREMENT unary_expression
									{ $$ = new Direct_Assignment($2, new Add_Expression($2, new Integer(1))); }

								|	T_DECREMENT unary_expression
									{ $$ = new Direct_Assignment($2, new Sub_Expression($2, new Integer(1))); }


cast_expression					: 	unary_expression 

								| 	T_LBRACKET TYPE T_RBRACKET cast_expression
									{ $$ = new Cast_Expression(*$2, $4); }

postfix_expression				:	primary_expression	

								|	postfix_expression T_INCREMENT											
									{ $$ = new Post_Increment_Expression($1, new Direct_Assignment($1, new Add_Expression($1, new Integer(1)))); }

								|	postfix_expression T_DECREMENT		
									{ $$ = new Post_Decrement_Expression($1, new Direct_Assignment($1, new Decrement_Expression($1, new Integer(1)))); }

								|	postfix_expression T_LBRACKET T_RBRACKET							
									{ $$ = new Function_Call_Expression($1) ; }

								|	postfix_expression T_LBRACKET argument_list T_RBRACKET				
									{ $$ = new Function_Call_Expression($1, $3); }

								|	postfix_expression T_SQUARE_LBRACKET expression T_SQUARE_RBRACKET	
									{ $$ = new Array_Access_Expression($1, $3); }


/* ------------------------------------						   Arthimetic Expression						------------------------------------ */

multiply_expression				:	cast_expression				 									
									{ $$ = $1; }

								| 	multiply_expression T_MULTIPLY cast_expression 					
									{ $$ = new Multiply_Expression($1, $3); }

								| 	multiply_expression T_DIVIDE postfix_expression 					
									{ $$ = new Divide_Expression($1, $3); }

add_expression					: 	multiply_expression					  								
									{ $$ = $1; }

								| 	add_expression T_PLUS multiply_expression							
									{ $$ = new Add_Expression($1, $3); }

								| 	add_expression T_MINUS multiply_expression  						
									{ $$ = new Sub_Expression($1, $3); }

/* ------------------------------------					      Bitwise Shift Expression						------------------------------------ */

bitwise_shift_expression		:	add_expression

								|	bitwise_shift_expression T_LEFT_SHIFT add_expression 
									{ $$ = new Left_Bitwise_Shift_Expression($1, $3); }

								|	bitwise_shift_expression T_RIGHT_SHIFT add_expression
									{ $$ = new Right_Bitwise_Shift_Expression($1, $3); }

compare_expression				: 	bitwise_shift_expression

								|	compare_expression T_LESS bitwise_shift_expression						
									{ $$ = new Less_Than_Expression($1, $3); }

								|	compare_expression T_LESS_EQUAL bitwise_shift_expression					
									{ $$ = new Less_Than_Equal_Expression($1, $3); }

								|	compare_expression T_GREATER bitwise_shift_expression					
									{ $$ = new More_Than_Expression($1, $3); }

								|	compare_expression T_GREATER_EQUAL bitwise_shift_expression				
									{ $$ = new More_Than_Equal_Expression($1, $3); }

equal_expression				: 	compare_expression 

								|	equal_expression T_EQUAL compare_expression							
									{ $$ = new Equal_Expression($1, $3); }

								|	equal_expression T_NOT_EQUAL compare_expression						
									{ $$ = new Not_Equal_Expression($1, $3); }


/* ------------------------------------						     Bitwise Expression							------------------------------------ */

bitwise_AND_expression			: 	equal_expression 

								| 	bitwise_AND_expression T_BITWISE_AND equal_expression				
									{ $$ = new Bitwise_AND_Expression($1, $3); }

bitwise_XOR_expression			: 	bitwise_AND_expression 

								| 	bitwise_XOR_expression T_BITWISE_XOR bitwise_AND_expression			
									{ $$ = new Bitwise_XOR_Expression($1, $3); }

bitwise_OR_expression			: 	bitwise_XOR_expression 

								| 	bitwise_OR_expression T_BITWISE_OR bitwise_XOR_expression			
									{ $$ = new Bitwise_OR_Expression($1, $3); }


/* ------------------------------------						     Logical Expression							------------------------------------ */

logical_AND_expression			:	bitwise_OR_expression

								|	logical_AND_expression T_LOGICAL_AND bitwise_OR_expression 			
									{ $$ = new Logical_AND_Expression($1, $3); }

logical_OR_expression			: 	logical_AND_expression

								|	logical_OR_expression T_LOGICAL_OR bitwise_XOR_expression			
									{ $$ = new Logical_OR_Expression($1, $3); }

ternary_expression 				: 	logical_OR_expression 


/* ------------------------------------						    Assignment Expression						------------------------------------ */

assignment_expression			: 	ternary_expression 

								|	unary_expression T_ASSIGN assignment_expression 					
									{ $$ = new Direct_Assignment($1, $3); }	

								|	unary_expression T_ADD_ASSIGN assignment_expression 					
									{ $$ = new Direct_Assignment($1, new Add_Expression($1, $3)); }	

								|	unary_expression T_SUB_ASSIGN assignment_expression 					
									{ $$ = new Direct_Assignment($1, new Sub_Expression($1, $3)); }	

argument_list					: 	assignment_expression
									{ $$ = new std::vector<Expression*>(1, $1); }

								| 	argument_list T_COMMA assignment_expression 									
									{ $1->push_back($3); $$ = $1; }


/* ------------------------------------								Statement								------------------------------------ */

statement 						: 	jump_statement														
									{ $$ = $1; }

								| 	compound_statement													
									{ $$ = $1; }

								| 	expression_statement												
									{ $$ = $1; }

								| 	condition_statement													
									{ $$ = $1; }

								|	iteration_statement													
									{ $$ = $1; }

								|	labeled_statement
									{ $$ = $1; }

statement_list 					: 	statement 															
									{ $$ = new std::vector<Statement*>(1, $1); }

								| 	statement_list statement											
									{ $1->push_back($2); $$ = $1; }

compound_statement  			: 	T_CURLY_LBRACKET T_CURLY_RBRACKET									
									{ $$ = new Compound_Statement(); }

								| 	T_CURLY_LBRACKET statement_list T_CURLY_RBRACKET					
									{ $$ = new Compound_Statement(NULL, $2); }

								| 	T_CURLY_LBRACKET declaration_list T_CURLY_RBRACKET					
									{ $$ = new Compound_Statement($2, NULL); }

								| 	T_CURLY_LBRACKET declaration_list statement_list T_CURLY_RBRACKET	
									{ $$ = new Compound_Statement($2, $3); }
					
jump_statement					: 	T_RETURN T_SEMICOLON												
									{ $$ = new Jump_Statement(); }

								|	T_BREAK T_SEMICOLON
									{ $$ = new Break_Statement(); }

								|	T_CONTINUE T_SEMICOLON
									{ $$ = new Continue_Statement(); }

								| 	T_RETURN expression T_SEMICOLON										
									{ $$ = new Jump_Statement($2); }

expression_statement			: 	expression T_SEMICOLON												
									{ $$ = new Expression_Statement($1); }

								|	T_SEMICOLON															
									{ $$ = new Expression_Statement(); }	

condition_statement 			:	T_IF T_LBRACKET expression T_RBRACKET statement						
									{ $$ = new Condition_If_Statement($3,$5); }	

								|	T_IF T_LBRACKET expression T_RBRACKET statement T_ELSE statement	
									{ $$ = new Condition_If_Else_Statement($3,$5,$7); }

								|	T_SWITCH T_LBRACKET expression T_RBRACKET statement	
									{ $$ = new Switch_Statement($3,$5); }

iteration_statement				:	T_WHILE T_LBRACKET expression T_RBRACKET statement 												
									{ $$ = new While_Statement($3,$5); }

								|	T_FOR T_LBRACKET expression T_SEMICOLON expression T_SEMICOLON expression T_RBRACKET statement	
									{ $$ = new For_Statement($3,$5,$7,$9); }

labeled_statement				:	T_CASE expression T_COLON statement
									{ $$ = new Case_Statement($2, $4); }

								| 	T_DEFAULT T_COLON statement
									{ $$ = new Default_Statement($3); } 
	

/* ------------------------------------								  Others								----------------------------------- */

TYPE							:	T_INT 		
									{ $$ = new type_definition(INT); }

								|	T_VOID		
									{ $$ = new type_definition(VOID); } 	

								|	T_CHAR		
									{ $$ = new type_definition(CHAR); } 		

								|	T_DOUBLE		
									{ $$ = new type_definition(DOUBLE); } 		

								|	T_FLOAT		
									{ $$ = new type_definition(FLOAT); } 	

								|	T_UNSIGNED
									{ $$ = new type_definition(UNSIGNED_INT); } 	

								|	T_UNSIGNED T_INT
									{ $$ = new type_definition(UNSIGNED_INT); } 	
								
								/* https://en.cppreference.com/w/c/language/enum */

								|	T_ENUM
									{ $$ = new type_definition(INT); } 

								| 	TYPE T_MULTIPLY 
									{ 									
									  $$ = $1; 
									}	
								
%%

const Node *root; 

const Node* parse()
{
	root = 0;
	yyparse ();
	return root;
}

