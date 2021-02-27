%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Expression *g_root;

  int yylex(void);
  void yyerror(const char *);
}

%union{
  const Expression *expr;
  double number;
  std::string *string;
}

// Assignment Operators
%token T_ASSIGN T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_INCREMENT T_DECREMENT
%token T_AND_ASSIGN T_OR_ASSIGN T_XOR_ASSIGN T_SHIFT_LEFT_ASSIGN T_SHIFT_RIGHT_ASSIGN
// Arithmetic Operators
%token T_MULTIPLY T_DIVIDE T_PLUS T_MINUS T_MODULO
// Bitwise Operators
%token T_BITWISE_NOT T_BITWISE_AND T_BITWISE_XOR T_BITWISE_OR T_BITWISE_SHIFT_LEFT T_BITWISE_SHIFT_RIGHT
// Logical Operators
%token T_LOGICAL_AND T_LOGICAL_OR T_LOGICAL_NOT
// Characters Operators
%token T_LBRACKET T_RBRACKET T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET T_COLON T_SEMICOLON
// Comparison Operators
%token T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL T_EQUAL T_NOT_EQUAL
// Types Operators
%token T_CHAR T_INT T_FLOAT T_DOUBLE T_UNSIGNED
// Structures Operators
%token T_IF T_ELSE T_SWITCH T_WHILE T_FOR T_CONTINUE T_BREAK T_RETURN
// Keywords Operators
%token T_TYPEDEF T_STRUCT T_ENUM T_SIZEOF
// Rules
%token T_IDENTIFIER T_CONSTANT

%start ROOT

%%

primary_expression
    : T_IDENTIFIER
    | T_CONSTANT
    | '(' expression ')'
    ;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' T_IDENTIFIER
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator unary_expression
	| T_SIZEOF unary_expression
	| T_SIZEOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	| multiplicative_expression '/' unary_expression
	| multiplicative_expression '%' unary_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression T_BITWISE_SHIFT_LEFT additive_expression
	| shift_expression T_BITWISE_SHIFT_RIGHT additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression T_LESS_EQUAL shift_expression
	| relational_expression T_GREATER_EQUAL shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression T_EQUAL relational_expression
	| equality_expression T_NOT_EQUAL relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression T_BITWISE_AND inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression T_BITWISE_OR logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: T_ASSIGN
  | T_ADD_ASSIGN
  | T_SUB_ASSIGN
	| T_MUL_ASSIGN
	| T_DIV_ASSIGN
	| T_MOD_ASSIGN
	| T_SHIFT_LEFT_ASSIGN
	| T_SHIFT_RIGHT_ASSIGN
	| T_AND_ASSIGN
  | T_OR_ASSIGN
	| T_XOR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: T_TYPEDEF
	| T_TYPEDEF declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator
	| declarator '=' initializer
	;

type_specifier
	: T_CHAR
	| T_INT
	| T_FLOAT
	| T_DOUBLE
	| T_UNSIGNED
	| struct_specifier
	| enum_specifier
	| T_TYPEIDENTIFIER
	;

struct_specifier
	: T_STRUCT T_IDENTIFIER '{' struct_declaration_list '}'
	| T_STRUCT '{' struct_declaration_list '}'
	| T_STRUCT T_IDENTIFIER
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: T_ENUM '{' enumerator_list '}'
	| T_ENUM T_IDENTIFIER '{' enumerator_list '}'
	| T_ENUM T_IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: T_IDENTIFIER
	| T_IDENTIFIER '=' constant_expression
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: T_IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

pointer
	: '*'
	| '*' pointer
	;

parameter_type_list
	: parameter_list
	| parameter_list ',' T_ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: T_IDENTIFIER
	| identifier_list ',' T_IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: T_IDENTIFIER ':' statement
	| T_CASE constant_expression ':' statement
	| T_DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: T_WHILE '(' expression ')' statement
	| T_DO statement T_WHILE '(' expression ')' ';'
	| T_FOR '(' expression_statement expression_statement ')' statement
	| T_FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: T_CONTINUE ';'
	| T_BREAK ';'
	| T_RETURN ';'
	| T_RETURN expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

%%
#include <stdio.h>

extern char yytext[];
extern int column;

yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

%%

const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
