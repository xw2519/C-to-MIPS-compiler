%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Node *root;

  int yylex(void);
  void yyerror(const char *);
}

%union{
  const Node *node;
  Statement *statement_node;
  std::string *string;
}

// Assignment Operators
%token T_ASSIGN T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_INCREMENT T_DECREMENT
%token T_AND_ASSIGN T_OR_ASSIGN T_XOR_ASSIGN T_SHIFT_LEFT_ASSIGN T_SHIFT_RIGHT_ASSIGN
// Arithmetic Operators
%token T_DIVIDE T_MULTIPLY T_MODULO T_PLUS T_MINUS
// Bitwise Operators
%token T_BITWISE_NOT T_BITWISE_AND T_BITWISE_XOR T_BITWISE_OR T_BITWISE_SHIFT_LEFT T_BITWISE_SHIFT_RIGHT
// Logical Operators
%token T_LOGICAL_AND T_LOGICAL_OR T_LOGICAL_NOT
// Characters Operators
%token T_LBRACKET T_RBRACKET T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET
%token T_ELLIPSIS T_COLON T_SEMICOLON T_DOT T_COMMA T_ARROW T_REFERENCE T_DEREFERENCE
// Comparison Operators
%token T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL T_EQUAL T_NOT_EQUAL
// Types Operators
%token T_CHAR T_INT T_FLOAT T_DOUBLE T_UNSIGNED
// Structures Operators
%token T_SWITCH T_WHILE T_FOR T_IF T_ELSE T_CONTINUE T_BREAK T_RETURN
// Keywords Operators
%token T_TYPEDEF T_STRUCT T_ENUM T_SIZEOF
// Rules
%token T_IDENTIFIER T_CONSTANT T_STRING_LITERAL

%start ROOT

%%

primary_expression : T_IDENTIFIER                                                                  { $$ = new PrimaryExpression(T_IDENTIFIER, $1); }
                   | T_CONSTANT                                                                    { $$ = new PrimaryExpression(T_CONSTANT, $1); }
                   | T_STRING_LITERAL                                                              { $$ = new PrimaryExpression(T_STRING_LITERAL, $1); }
                   | T_LBRACKET assignment_expression T_RBRACKET                                   { $$ = new PrimaryExpression($2); }

postfix_expression : primary_expression                                                            { $$ = new PostfixExpression($1); }
                   | postfix_expression T_SQUARE_LBRACKET assignment_expression T_SQUARE_RBRACKET  { $$ = new ArrayPostfixExpression($1, $3); }
                   | postfix_expression T_LBRACKET T_RBRACKET                                      { $$ = new FunctionPostfixExpression($1); }
                   | postfix_expression T_LBRACKET argument_expression_list T_RBRACKET             { $$ = new FunctionPostfixExpression($1, $3); }
                   | postfix_expression T_DOT T_IDENTIFIER                                         { $$ = new PostfixExpression(T_DOT, $1, $3); }
                   | postfix_expression T_ARROW T_IDENTIFIER                                       { $$ = new PostfixExpression(T_ARROW, $1, $3); }
                   | postfix_expression T_INCREMENT                                                { $$ = new PostfixExpression(T_INCREMENT, $1); }
                   | postfix_expression T_DECREMENT                                                { $$ = new PostfixExpression(T_DECREMENT, $1); }

argument_expression_list : assignment_expression                                                   { $$ = new ArgumentExpressionList($1); }
                         | argument_expression_list T_COMMA assignment_expression                  { $$ = new ArgumentExpressionList($1, $3); }

unary_expression : postfix_expression                                                              { $$ = new UnaryExpression($1); }
                 | T_INCREMENT unary_expression                                                    { $$ = new UnaryExpression(T_INCREMENT, $2); }
                 | T_DECREMENT unary_expression                                                    { $$ = new UnaryExpression(T_DECREMENT, $2); }
                 | unary_operator unary_expression                                                 { $$ = new UnaryExpression($1, $2); }
                 | T_SIZEOF unary_expression                                                       { $$ = new UnaryExpression(T_SIZEOF, $2); }
                 | T_SIZEOF T_LBRACKET type_name T_RBRACKET                                        { $$ = new UnaryExpression(T_SIZEOF, $3); }

unary_operator : T_BITWISE_AND                                                                     { $$ = new UnaryOperator(T_REFERENCE); }
               | T_MULTIPLY                                                                        { $$ = new UnaryOperator(T_DEREFERENCE); }
               | T_PLUS                                                                            { $$ = new UnaryOperator(T_PLUS); }
               | T_MINUS                                                                           { $$ = new UnaryOperator(T_MINUS); }
               | T_BITWISE_NOT                                                                     { $$ = new UnaryOperator(T_BITWISE_NOT); }
               | T_LOGICAL_NOT                                                                     { $$ = new UnaryOperator(T_LOGICAL_NOT); }

multiplicative_expression : unary_expression                                                       { $$ = new MultiplicativeExpression($1); }
                          | multiplicative_expression T_MULTIPLY unary_expression                  { $$ = new MultiplicativeExpression(T_MULTIPLY, $1, $3); }
                          | multiplicative_expression T_DIVIDE unary_expression                    { $$ = new MultiplicativeExpression(T_DIVIDE, $1, $3); }
                          | multiplicative_expression T_MODULO unary_expression                    { $$ = new MultiplicativeExpression(T_MODULO, $1, $3); }

additive_expression : multiplicative_expression                                                    { $$ = new AdditiveExpression($1); }
                    | additive_expression T_PLUS multiplicative_expression                         { $$ = new AdditiveExpression(T_PLUS, $1, $3); }
                    | additive_expression T_MINUS multiplicative_expression                        { $$ = new AdditiveExpression(T_MINUS, $1, $3); }

shift_expression : additive_expression                                                             { $$ = new ShiftExpression($1); }
                 | shift_expression T_BITWISE_SHIFT_LEFT additive_expression                       { $$ = new ShiftExpression(T_BITWISE_SHIFT_LEFT, $1, $3); }
                 | shift_expression T_BITWISE_SHIFT_RIGHT additive_expression                      { $$ = new ShiftExpression(T_BITWISE_SHIFT_RIGHT, $1, $3); }

relational_expression : shift_expression                                                           { $$ = new RelationalExpression($1); }
                      | relational_expression T_LESS shift_expression                              { $$ = new RelationalExpression(T_LESS, $1, $3); }
                      | relational_expression T_GREATER shift_expression                           { $$ = new RelationalExpression(T_GREATER, $1, $3); }
                      | relational_expression T_LESS_EQUAL shift_expression                        { $$ = new RelationalExpression(T_LESS_EQUAL, $1, $3); }
                      | relational_expression T_GREATER_EQUAL shift_expression                     { $$ = new RelationalExpression(T_GREATER_EQUAL, $1, $3); }

equality_expression : relational_expression                                                        { $$ = new EqualityExpression($1); }
                    | equality_expression T_EQUAL relational_expression                            { $$ = new EqualityExpression(T_EQUAL, $1, $3); }
                    | equality_expression T_NOT_EQUAL relational_expression                        { $$ = new EqualityExpression(T_NOT_EQUAL, $1, $3); }

and_expression : equality_expression
               | and_expression T_BITWISE_AND equality_expression

exclusive_or_expression : and_expression
                        | exclusive_or_expression T_BITWISE_XOR and_expression

inclusive_or_expression : exclusive_or_expression
                        | inclusive_or_expression T_BITWISE_OR exclusive_or_expression

logical_and_expression : inclusive_or_expression
                       | logical_and_expression T_BITWISE_AND inclusive_or_expression

constant_expression : logical_and_expression
                    | constant_expression T_BITWISE_OR logical_and_expression

assignment_expression : constant_expression
                      | unary_expression assignment_operator assignment_expression

assignment_operator : T_ASSIGN
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
	| struct_declarator_list T_COMMA struct_declarator
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
	| enumerator_list T_COMMA enumerator
	;

enumerator
	: T_IDENTIFIER
	| T_IDENTIFIER '=' constant_expression
	;

pointer : '*'                                                                                      { $$ = new Pointer(); }
        | '*' pointer                                                                              { $$ = new Pointer($2); }

parameter_type_list
	: parameter_list
	| parameter_list T_COMMA T_ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list T_COMMA parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: T_IDENTIFIER
	| identifier_list T_COMMA T_IDENTIFIER
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


compound_statement : T_CURLY_LBRACKET T_CURLY_RBRACKET                                             { $$ = new CompoundStatement(); }
                   | T_CURLY_LBRACKET statement_list T_CURLY_RBRACKET                              { $$ = new CompoundStatement($2); }
                   | T_CURLY_LBRACKET declaration_list T_CURLY_RBRACKET                            { $$ = new CompoundStatement($2); }
                   | T_CURLY_LBRACKET declaration_list statement_list T_CURLY_RBRACKET             { $$ = new CompoundStatement($2, $3); }

declaration_list : declaration                                                                     { $$ = new DeclarationList($1); }
                 | declaration_list declaration                                                    { $$ = new DeclarationList($1); }

statement_list : statement                                                                         { $$ = new StatementList($1); }
               | statement_list statement                                                          { $$ = new StatementList($1); }

statement : labeled_statement                                                                      { $$ = $1; }
          | compound_statement                                                                     { $$ = $1; }
          | expression_statement                                                                   { $$ = $1; }
          | selection_statement                                                                    { $$ = $1; }
          | iteration_statement                                                                    { $$ = $1; }
          | jump_statement                                                                         { $$ = $1; }

labeled_statement : T_CASE constant_expression T_COLON statement                                   { $$ = new Statement(T_CASE, $2, $4); }
                  | T_DEFAULT T_COLON statement                                                    { $$ = new Statement(T_DEFAULT, $3); }

expression_statement : T_SEMICOLON                                                                 { $$ = new Statement(T_SEMICOLON); }
                     | assignment_expression T_SEMICOLON                                           { $$ = new Statement(T_SEMICOLON, $1); }

selection_statement : T_IF T_LBRACKET assignment_expression T_RBRACKET statement                   { $$ = new Statement(T_IF, $3, $5); }
                    | T_IF T_LBRACKET assignment_expression T_RBRACKET statement T_ELSE statement  { $$ = new Statement(T_ELSE, $3, $5, $7); }
                    | T_SWITCH T_LBRACKET assignment_expression T_RBRACKET statement               { $$ = new Statement(T_SWITCH, $3, $5); }

iteration_statement : T_WHILE T_LBRACKET assignment_expression T_RBRACKET statement                                         { $$ = new Statement(T_DO, $3, $4, $6); }
                    | T_DO statement T_WHILE T_LBRACKET assignment_expression T_RBRACKET T_SEMICOLON                        { $$ = new Statement(T_WHILE, $2, $5); }
                    | T_FOR T_LBRACKET expression_statement expression_statement T_RBRACKET statement                       { $$ = new Statement(T_FOR, $3, $4, $6); }
                    | T_FOR T_LBRACKET expression_statement expression_statement assignment_expression T_RBRACKET statement { $$ = new Statement(T_FOR, $3, $4, $5, $7); }

jump_statement : T_CONTINUE T_SEMICOLON                                                            { $$ = new Statement(T_CONTINUE); }
               | T_BREAK T_SEMICOLON                                                               { $$ = new Statement(T_BREAK); }
               | T_RETURN T_SEMICOLON                                                              { $$ = new Statement(T_RETURN); }
               | T_RETURN assignment_expression T_SEMICOLON                                        { $$ = new Statement(T_RETURN, $2); }


ROOT : translation_unit                                                                            { root = $1; }

translation_unit : external_declaration                                                            { $$ = new TranslationUnit($1); }
                 | translation_unit external_declaration                                           { $$ = new TranslationUnit($1, $2); }

external_declaration : function_definition                                                         { $$ = $1; }
                     | declaration                                                                 { $$ = $1; }

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
	| init_declarator_list T_COMMA init_declarator
	;

type_specifier : T_CHAR                                                                            { $$ = new PrimitiveType(T_CHAR); }
               | T_INT                                                                             { $$ = new PrimitiveType(T_INT); }
               | T_FLOAT                                                                           { $$ = new PrimitiveType(T_FLOAT); }
               | T_DOUBLE                                                                          { $$ = new PrimitiveType(T_DOUBLE); }
               | T_UNSIGNED                                                                        { $$ = new PrimitiveType(T_UNSIGNED); }
               | T_TYPEIDENTIFIER                                                                  { $$ = new TypeIDType($1); }
               | struct_specifier                                                                  { $$ = new StructType($1); }
               | enum_specifier                                                                    { $$ = new EnumType($1); }

init_declarator
	: declarator
	| declarator '=' initializer
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list T_COMMA '}'
	;

initializer_list
	: initializer
	| initializer_list T_COMMA initializer
	;

function_definition : declaration_specifiers declarator compound_statement                         { $$ = new FunctionDefinition($1, $2, $3); }

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

const Node *root; // Definition of variable (to match declaration earlier)

const Node *parse()
{
  root=0;
  yyparse();
  return root;
}
