%code requires{
#include "ast.hpp"
#include <cassert>

extern const Node *root; // Definition of variable (to match declaration earlier)

T_INT yylex(void);
void yyerror(const char *);
}

%union{
	const Node 	*node;
	Statement 	*statement_node;
	std::string *string;
}

%token T_INT
%token T_RETURN
%token T_IDENTIFIER


%type <node> translation_unit function_definition
%type <statement_node> jump_statement statement statement_list compound_statement 
%type <string> type_specifier 
%type <string> T_IDENTIFIER T_INT T_RETURN 

%start ROOT

%%

ROOT 				: translation_unit							{ root = $1; }

translation_unit 	: function_definition					  	{ $$ = $1; }
					| translation_unit function_definition   	{ $$ = new TranslationUnit($1,$2); }

function_definition : type_specifier T_IDENTIFIER '(' parameter_list ')' compound_statement { $$ = new FunctionDefinition(*$1,*$2,$6); }

parameter_list 		:

compound_statement  : '{' statement_list '}'	{ $$ = $2; }
					| '{' '}'					{ $$ = NULL; }

statement_list 		: statement 				{ $$ = $1; }
					| statement_list statement 	

statement 			: jump_statement			{ $$ = $1; }
					| compound_statement	
				
jump_statement		: T_RETURN ';'  			{ $$ = new JumpStatement("T_RETURN"); }

type_specifier		: T_INT 		  			{ $$ = new std::string("T_INT"); }

%%

const Node *root; // Definition of variable (to match declaration earlier)

const Node* parse(){
	root = 0;
	yyparse();
	T_RETURN root;
}