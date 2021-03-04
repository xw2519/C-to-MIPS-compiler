/*

Contents and terms are derived from the ANSI C programming language (ANSI/ISO 9899/1990) specification

*/

%define parse.error verbose 

%code requires{
#include "ast.hpp"
#include <cassert>
#include <vector>

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

	std::vector<Expression*>* 	argument_list_vector;
	std::vector<Statement*>* 	statement_list_vector;
	std::vector<Declaration*>* 	declaration_list_vector;
	std::vector<Declarator*>* 	declarator_list_vector;
}

/* ------------------------------------					Tokens					------------------------------------ */

// Assignment Operators
%token T_ASSIGN T_INCREMENT 
// Arithmetic Operators
%token T_MULTIPLY T_DIVIDE T_PLUS T_MINUS T_MODULO
// Characters Operators
%token T_LBRACKET T_RBRACKET T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET T_COLON T_SEMICOLON T_COMMA
// Comparison Operators
%token T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL T_EQUAL T_NOT_EQUAL
// Types Operators
%token T_INT T_VOID
// Structures Operators
%token T_IF T_ELSE T_SWITCH T_WHILE T_FOR T_CONTINUE T_BREAK T_RETURN
// Rules
%token T_IDENTIFIER T_CONSTANT 

%token INC_OP T_LITERAL

/* ------------------------------------					Type					------------------------------------ */

%type <node> program 
%type <node> function_definition
%type <node> global_declaration

%type <declarator_node>	declarator initialisation_declarator
%type <declarator_list_vector> initialisation_declarator_list

%type <declaration_node> declaration parameter_declaration 
%type <declaration_list_vector> declaration_list parameter_list

%type <expression_node> primary_expression postfix_expression 
%type <expression_node> multiply_expression add_expression 
%type <expression_node> bw_shift_expression compare_expression equal_expression
%type <expression_node> bitwise_expression logical_expression ternary_expression 
%type <expression_node> assignment_expression expression 


%type <argument_list_vector> argument_list

%type <statement_node> statement 
%type <statement_node> jump_statement compound_statement expression_statement condition_statement iteration_statement

%type <statement_list_vector> statement_list

%type <string> 	T_IDENTIFIER T_INT T_RETURN T_LITERAL
%type <string> 	T_TYPE 

%type <int_num> T_CONSTANT


%start ROOT

%%

/* ------------------------------------							 Base definitions							------------------------------------ */

ROOT 							: 	program														{ root = $1; }

program 						: 	global_declaration					  						{ $$ = $1; }
								| 	program function_definition   								{ $$ = new Program($1,$2); }

global_declaration				:	function_definition											{ $$ = $1; }
								|	declaration 												{ $$ = $1; }

function_definition				:	T_TYPE T_IDENTIFIER T_LBRACKET parameter_list T_RBRACKET compound_statement 
									{ $$ = new Function_Definition(*$1, *$2, $4, $6); }

declarator						: 	T_IDENTIFIER 												{ $$ = new Declarator(*$1); }

initialisation_declarator		: 	declarator 													{ $$ = $1; }
								| 	declarator T_EQUAL expression 								{ $$ = new Declarator($1->getID(), $3); }

initialisation_declarator_list	: 	initialisation_declarator 											{ $$ = new std::vector<Declarator*>(1, $1);	}
								|	initialisation_declarator_list T_COMMA initialisation_declarator	{ $1->push_back($3); $$ = $1; }

declaration 					:	T_TYPE T_SEMICOLON											{ $$ = new Declaration(*$1); }
								| 	T_TYPE initialisation_declarator_list T_SEMICOLON 			{ $$ = new Declaration(*$1, $2); }

declaration_list				: 	declaration 												{ $$ = new std::vector<Declaration*>(1, $1); } 
								| 	declaration_list declaration  								{ $1->push_back($2); $$ = $1; }

parameter_declaration			:	T_TYPE declarator 											{ $$ = new Declaration(*$1, new std::vector<Declarator*>(1, $2)); }	

parameter_list					:	parameter_declaration				     					{ $$ = new std::vector<Declaration*>(1, $1); }
								|	parameter_list T_COMMA parameter_declaration 				{ $1->push_back($3); $$ = $1; }
								|											 					{ $$ = NULL; }


/* ------------------------------------								Expression								------------------------------------ */

expression 						:	assignment_expression

primary_expression				: 	T_CONSTANT														{ $$ = new Constant($1); }
								| 	T_IDENTIFIER		 											{ $$ = new Variable(*$1);	}	
								| 	T_LITERAL			 											{ $$ = new StringLiteral(*$1); }	
								| 	T_LBRACKET expression T_RBRACKET								{ $$ = $2; }		

postfix_expression				:	primary_expression												{ $$ = $1; }
								|	primary_expression INC_OP										{ $$ = new Post_Increment_Expression($1); }
								|	primary_expression T_LBRACKET T_RBRACKET						{ $$ = new Function_Call_Expression($1) ; }
								|	primary_expression T_LBRACKET argument_list T_RBRACKET			{ $$ = new Function_Call_Expression($1, $3); }

add_expression					: 	multiply_expression					  							{ $$ = $1; }
								| 	add_expression T_PLUS multiply_expression						{ $$ = new Add_Expression($1, $3); }
								| 	add_expression T_MINUS multiply_expression  					{ $$ = new Sub_Expression($1, $3); }

multiply_expression				:	postfix_expression				 								{ $$ = $1; }
								| 	multiply_expression T_MULTIPLY postfix_expression 				{ $$ = new Multiply_Expression($1, $3); }
								| 	multiply_expression T_DIVIDE postfix_expression 				{ $$ = new Divide_Expression($1, $3); }
 
bw_shift_expression				:	add_expression

compare_expression				: 	bw_shift_expression
								|	compare_expression T_LESS bw_shift_expression					{ $$ = new Less_Than_Expression($1, $3); }
								|	compare_expression T_LESS_EQUAL bw_shift_expression				{ $$ = new Less_Than_Equal_Expression($1, $3); }
								|	compare_expression T_GREATER bw_shift_expression				{ $$ = new More_Than_Expression($1, $3); }
								|	compare_expression T_GREATER_EQUAL bw_shift_expression			{ $$ = new More_Than_Equal_Expression($1, $3); }

equal_expression				: 	compare_expression 
								|	equal_expression T_EQUAL compare_expression						{ $$ = new Equal_Expression($1, $3); }
								|	equal_expression T_NOT_EQUAL compare_expression					{ $$ = new Not_Equal_Expression($1, $3); }

bitwise_expression				: 	equal_expression 

logical_expression				: 	bitwise_expression

ternary_expression 				: 	logical_expression 

assignment_expression			: 	ternary_expression 
								|	postfix_expression T_ASSIGN assignment_expression 				{ $$ = new Direct_Assignment($1, $3); }	

argument_list					: 	expression 														{ $$ = new std::vector<Expression*>(1,$1); }
								| 	argument_list T_COMMA expression 								{ $1->push_back($3); $$ = $1; }


/* ------------------------------------								Statement								------------------------------------ */

statement 						: 	jump_statement														{ $$ = $1; }
								| 	compound_statement													{ $$ = $1; }
								| 	expression_statement												{ $$ = $1; }
								| 	condition_statement													{ $$ = $1; }
								|	iteration_statement													{ $$ = $1; }


statement_list 					: 	statement 															{ $$ = new std::vector<Statement*>(1, $1); }
								| 	statement_list statement											{ $1->push_back($2); $$ = $1; }


compound_statement  			: 	T_CURLY_LBRACKET T_CURLY_RBRACKET									{ $$ = new Compound_Statement(); }
								| 	T_CURLY_LBRACKET statement_list T_CURLY_RBRACKET					{ $$ = new Compound_Statement(NULL, $2); }
								| 	T_CURLY_LBRACKET declaration_list T_CURLY_RBRACKET					{ $$ = new Compound_Statement($2, NULL); }
								| 	T_CURLY_LBRACKET declaration_list statement_list T_CURLY_RBRACKET	{ $$ = new Compound_Statement($2, $3); }
					
jump_statement					: 	T_RETURN T_SEMICOLON												{ $$ = new Jump_Statement(); }
								| 	T_RETURN expression T_SEMICOLON										{ $$ = new Jump_Statement($2); }

expression_statement			: 	expression T_SEMICOLON												{ $$ = new Expression_Statement($1); }


condition_statement 			:	T_IF T_LBRACKET expression T_RBRACKET statement						{ $$ = new Condition_If_Statement($3,$5); }	
								|	T_IF T_LBRACKET expression T_RBRACKET statement T_ELSE statement	{ $$ = new Condition_If_Else_Statement($3,$5,$7); }

iteration_statement				:	T_WHILE T_LBRACKET expression T_RBRACKET statement 												{ $$ = new While_Statement($3,$5); }
								|	T_FOR T_LBRACKET expression T_SEMICOLON expression T_SEMICOLON expression T_RBRACKET statement	{ $$ = new For_Statement($3,$5,$7,$9); }


/* ------------------------------------								Others									------------------------------------ */

T_TYPE							:	T_INT 		{ $$ = new std::string("int"); }
								|	T_VOID		{ $$ = new std::string("void"); } 			

%%


const Node *root; 

const Node* parse()
{
	root = 0;
	yyparse ();
	return root;
}

