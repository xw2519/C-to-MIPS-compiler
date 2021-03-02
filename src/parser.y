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
%token T_COLON T_SEMICOLON T_DOT T_COMMA T_ARROW T_REFERENCE T_DEREFERENCE
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
/* Expressions */
primary_expression : T_IDENTIFIER                                                                  { $$ = new Identifier(*$1); }
                   | T_CONSTANT                                                                    { $$ = new IntegralConstant(*$1); }
                   | T_FLOAT_CONSTANT                                                              { $$ = new FloatConstant(*$1); }
                   | T_STRING_LITERAL                                                              { $$ = new StringLiteral(*$1); }
                   | T_LBRACKET assignment_expression T_RBRACKET                                   { $$ = $2; }

postfix_expression : primary_expression                                                            { $$ = $1; }
                   | postfix_expression T_SQUARE_LBRACKET assignment_expression T_SQUARE_RBRACKET  { $$ = new ArrayPostfixExpression($1, $3); }
                   | postfix_expression T_LBRACKET T_RBRACKET                                      { $$ = new FunctionPostfixExpression($1); }
                   | postfix_expression T_LBRACKET argument_expression_list T_RBRACKET             { $$ = new FunctionPostfixExpression($1, $3); }
                   | postfix_expression T_DOT T_IDENTIFIER                                         { $$ = new PostfixExpression(DOT, $1, *$3); }
                   | postfix_expression T_ARROW T_IDENTIFIER                                       { $$ = new PostfixExpression(ARROW, $1, *$3); }
                   | postfix_expression T_INCREMENT                                                { $$ = new PostfixExpression(INCREMENT, $1); }
                   | postfix_expression T_DECREMENT                                                { $$ = new PostfixExpression(DECREMENT, $1); }

argument_expression_list : assignment_expression                                                   { $$ = new ArgumentExpressionList($1); }
                         | argument_expression_list T_COMMA assignment_expression                  { $$ = new ArgumentExpressionList($1, $3); }

unary_expression : postfix_expression                                                              { $$ = new UnaryExpression($1); }
                 | T_INCREMENT unary_expression                                                    { $$ = new UnaryExpression(INCREMENT, $2); }
                 | T_DECREMENT unary_expression                                                    { $$ = new UnaryExpression(DECREMENT, $2); }
                 | T_BITWISE_AND unary_expression                                                  { $$ = new UnaryExpression(REFERENCE, $2); }
                 | T_MULTIPLY unary_expression                                                     { $$ = new UnaryExpression(DEREFERENCE, $2); }
                 | T_PLUS unary_expression                                                         { $$ = $2; }
                 | T_MINUS unary_expression                                                        { $$ = new UnaryExpression(MINUS, $2); }
                 | T_BITWISE_NOT unary_expression                                                  { $$ = new UnaryExpression(BITWISE_NOT, $2); }
                 | T_LOGICAL_NOT unary_expression                                                  { $$ = new UnaryExpression(LOGICAL_NOT, $2); }
                 | T_SIZEOF unary_expression                                                       { $$ = new UnaryExpression(SIZEOF, $2); }
                 | T_SIZEOF T_LBRACKET type_name T_RBRACKET                                        { $$ = new UnaryExpression(SIZEOF, $3); }

multiplicative_expression : unary_expression                                                       { $$ = new MultiplicativeExpression($1); }
                          | multiplicative_expression T_MULTIPLY unary_expression                  { $$ = new MultiplicativeExpression(MULTIPLY, $1, $3); }
                          | multiplicative_expression T_DIVIDE unary_expression                    { $$ = new MultiplicativeExpression(DIVIDE, $1, $3); }
                          | multiplicative_expression T_MODULO unary_expression                    { $$ = new MultiplicativeExpression(MODULO, $1, $3); }

additive_expression : multiplicative_expression                                                    { $$ = new AdditiveExpression($1); }
                    | additive_expression T_PLUS multiplicative_expression                         { $$ = new AdditiveExpression(PLUS, $1, $3); }
                    | additive_expression T_MINUS multiplicative_expression                        { $$ = new AdditiveExpression(MINUS, $1, $3); }

shift_expression : additive_expression                                                             { $$ = new ShiftExpression($1); }
                 | shift_expression T_BITWISE_SHIFT_LEFT additive_expression                       { $$ = new ShiftExpression(BITWISE_SHIFT_LEFT, $1, $3); }
                 | shift_expression T_BITWISE_SHIFT_RIGHT additive_expression                      { $$ = new ShiftExpression(BITWISE_SHIFT_RIGHT, $1, $3); }

relational_expression : shift_expression                                                           { $$ = new RelationalExpression($1); }
                      | relational_expression T_LESS shift_expression                              { $$ = new RelationalExpression(LESS, $1, $3); }
                      | relational_expression T_GREATER shift_expression                           { $$ = new RelationalExpression(GREATER, $1, $3); }
                      | relational_expression T_LESS_EQUAL shift_expression                        { $$ = new RelationalExpression(LESS_EQUAL, $1, $3); }
                      | relational_expression T_GREATER_EQUAL shift_expression                     { $$ = new RelationalExpression(GREATER_EQUAL, $1, $3); }

equality_expression : relational_expression                                                        { $$ = new EqualityExpression($1); }
                    | equality_expression T_EQUAL relational_expression                            { $$ = new EqualityExpression(EQUAL, $1, $3); }
                    | equality_expression T_NOT_EQUAL relational_expression                        { $$ = new EqualityExpression(NOT_EQUAL, $1, $3); }

and_expression : equality_expression                                                               { $$ = new AndExpression($1); }
               | and_expression T_BITWISE_AND equality_expression                                  { $$ = new AndExpression($1, $3); }

exclusive_or_expression : and_expression                                                           { $$ = new ExclusiveOrExpression($1); }
                        | exclusive_or_expression T_BITWISE_XOR and_expression                     { $$ = new ExclusiveOrExpression($1, $3); }

inclusive_or_expression : exclusive_or_expression                                                  { $$ = new InclusiveOrExpression($1); }
                        | inclusive_or_expression T_BITWISE_OR exclusive_or_expression             { $$ = new InclusiveOrExpression($1, $3); }

logical_and_expression : inclusive_or_expression                                                   { $$ = new LogicalAndExpression($1); }
                       | logical_and_expression T_BITWISE_AND inclusive_or_expression              { $$ = new LogicalAndExpression($1, $3); }

constant_expression : logical_and_expression                                                       { $$ = new ConstantExpression($1); }
                    | constant_expression T_BITWISE_OR logical_and_expression                      { $$ = new ConstantExpression($1, $3); }

assignment_expression : constant_expression                                                        { $$ = new AssignmentExpression($1); }
                      | unary_expression T_ASSIGN assignment_expression                            { $$ = new AssignmentExpression(ASSIGN, $1, $3); }
                      | unary_expression T_ADD_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(ADD_ASSIGN, $1, $3); }
                      | unary_expression T_SUB_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(SUB_ASSIGN, $1, $3); }
                      | unary_expression T_MUL_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(MUL_ASSIGN, $1, $3); }
                      | unary_expression T_DIV_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(DIV_ASSIGN, $1, $3); }
                      | unary_expression T_MOD_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(MOD_ASSIGN, $1, $3); }
                      | unary_expression T_SHIFT_LEFT_ASSIGN assignment_expression                 { $$ = new AssignmentExpression(SHIFT_LEFT_ASSIGN, $1, $3); }
                      | unary_expression T_SHIFT_RIGHT_ASSIGN assignment_expression                { $$ = new AssignmentExpression(SHIFT_RIGHT_ASSIGN, $1, $3); }
                      | unary_expression T_AND_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(AND_ASSIGN, $1, $3); }
                      | unary_expression T_OR_ASSIGN assignment_expression                         { $$ = new AssignmentExpression(OR_ASSIGN, $1, $3); }
                      | unary_expression T_XOR_ASSIGN assignment_expression                        { $$ = new AssignmentExpression(XOR_ASSIGN, $1, $3); }

/* Structs and Enums */
struct_specifier : T_STRUCT T_IDENTIFIER T_CURLY_LBRACKET struct_declaration_list T_CURLY_RBRACKET { $$ = new StructSpecifier(*$2, $4); }
                 | T_STRUCT T_CURLY_LBRACKET struct_declaration_list T_CURLY_RBRACKET              { $$ = new StructSpecifier($3); }
                 | T_STRUCT T_IDENTIFIER                                                           { $$ = new StructSpecifier(*$2); }

struct_declaration_list : struct_declaration                                                       { $$ = new StructDeclarationList($1); }
                        | struct_declaration_list struct_declaration                               { $$ = new StructDeclarationList($1, $2); }

struct_declaration : specifier_qualifier_list struct_declarator_list T_SEMICOLON                   { $$ = new StructDeclaration($1, $2); }

specifier_qualifier_list : type_specifier                                                          { $$ = new SpecifierQualifierList($1); }
                         | type_specifier specifier_qualifier_list                                 { $$ = new SpecifierQualifierList($1, $2); }

struct_declarator_list : struct_declarator                                                         { $$ = new StructDeclatatorList($1); }
                       | struct_declarator_list T_COMMA struct_declarator                          { $$ = new StructDeclatatorList($1, $3); }

struct_declarator : declarator                                                                     { $$ = new StructDeclatator($1); }
                  | T_COLON constant_expression                                                    { $$ = new StructDeclatator($2); }
                  | declarator T_COLON constant_expression                                         { $$ = new StructDeclatator($1, $3); }

enum_specifier : T_ENUM T_CURLY_LBRACKET enumerator_list T_CURLY_RBRACKET                          { $$ = new EnumSpecifier($3); }
               | T_ENUM T_IDENTIFIER T_CURLY_LBRACKET enumerator_list T_CURLY_RBRACKET             { $$ = new EnumSpecifier(*$2, $4); }
               | T_ENUM T_IDENTIFIER                                                               { $$ = new EnumSpecifier(*$2); }

enumerator_list : enumerator                                                                       { $$ = new EnumeratorList($1); }
                | enumerator_list T_COMMA enumerator                                               { $$ = new EnumeratorList($1, $3); }

enumerator : T_IDENTIFIER                                                                          { $$ = new Enumerator(*$1); }
           | T_IDENTIFIER T_ASSIGN constant_expression                                             { $$ = new Enumerator(*$1, $3); }

parameter_list : parameter_declaration                                                             { $$ = new ParameterList($1); }
               | parameter_list T_COMMA parameter_declaration                                      { $$ = new ParameterList($1, $3); }

parameter_declaration : declaration_specifiers declarator                                          { $$ = new ParameterDeclaration($1, $2); }
                      | declaration_specifiers abstract_declarator                                 { $$ = new ParameterDeclaration($1, $2); }
                      | declaration_specifiers                                                     { $$ = new ParameterDeclaration($1); }

identifier_list : T_IDENTIFIER                                                                     { $$ = new IdentifierList(*$1); }
                | identifier_list T_COMMA T_IDENTIFIER                                             { $$ = new IdentifierList($1, *$3); }

type_name : specifier_qualifier_list                                                               { $$ = new TypeName($1); }
          | specifier_qualifier_list abstract_declarator                                           { $$ = new TypeName($1, $2); }

abstract_declarator : pointer                                                                      { $$ = new AbstractDeclarator($1); }
                    | direct_abstract_declarator                                                   { $$ = new AbstractDeclarator($1); }
                    | pointer direct_abstract_declarator                                           { $$ = new AbstractDeclarator($1, $2); }

direct_abstract_declarator : T_LBRACKET abstract_declarator T_RBRACKET                                            { $$ = new DirectAbstractDeclarator($2); }
                           | T_SQUARE_LBRACKET T_SQUARE_RBRACKET                                                  { $$ = new DirectAbstractDeclarator(SQUARE); }
                           | T_SQUARE_LBRACKET constant_expression T_SQUARE_RBRACKET                              { $$ = new DirectAbstractDeclarator($2); }
                           | direct_abstract_declarator T_SQUARE_LBRACKET T_SQUARE_RBRACKET                       { $$ = new DirectAbstractDeclarator(SQUARE, $1); }
                           | direct_abstract_declarator T_SQUARE_LBRACKET constant_expression T_SQUARE_RBRACKET   { $$ = new DirectAbstractDeclarator($1, $3); }
                           | T_LBRACKET T_RBRACKET                                                                { $$ = new DirectAbstractDeclarator(ROUND); }
                           | T_LBRACKET parameter_list T_RBRACKET                                                 { $$ = new DirectAbstractDeclarator($2); }
                           | direct_abstract_declarator T_LBRACKET T_RBRACKET                                     { $$ = new DirectAbstractDeclarator(ROUND, $1); }
                           | direct_abstract_declarator T_LBRACKET parameter_list T_RBRACKET                      { $$ = new DirectAbstractDeclarator($1, $3); }

pointer : T_MULTIPLY                                                                               { $$ = new Pointer(); }
        | T_MULTIPLY pointer                                                                       { $$ = new Pointer($2); }

/* Statements */
compound_statement : T_CURLY_LBRACKET T_CURLY_RBRACKET                                             { $$ = new Statement(COMPOUND); }
                   | T_CURLY_LBRACKET statement_list T_CURLY_RBRACKET                              { $$ = new Statement(COMPOUND, vector<Declaration*>*, $2); }
                   | T_CURLY_LBRACKET declaration_list T_CURLY_RBRACKET                            { $$ = new Statement(COMPOUND, $2, vector<Statement*>*); }
                   | T_CURLY_LBRACKET declaration_list statement_list T_CURLY_RBRACKET             { $$ = new Statement(COMPOUND, $2, $3); }

declaration_list : declaration                                                                     { $$ = new std::vector<Declaration*>{$1}; }
                 | declaration_list declaration                                                    { $1->push_back($2); $$ = $1; }

statement_list : statement                                                                         { $$ = new std::vector<Statement*>{$1}; }
               | statement_list statement                                                          { $1->push_back($2); $$ = $1; }

statement : labeled_statement                                                                      { $$ = $1; }
          | compound_statement                                                                     { $$ = $1; }
          | expression_statement                                                                   { $$ = $1; }
          | selection_statement                                                                    { $$ = $1; }
          | iteration_statement                                                                    { $$ = $1; }
          | jump_statement                                                                         { $$ = $1; }

labeled_statement : T_CASE constant_expression T_COLON statement                                   { $$ = new Statement(CASE, $2, $4); }
                  | T_DEFAULT T_COLON statement                                                    { $$ = new Statement(DEFAULT, $3); }

expression_statement : T_SEMICOLON                                                                 { $$ = new Statement(EXPR_STMT); }
                     | assignment_expression T_SEMICOLON                                           { $$ = new Statement(EXPR_STMT, $1); }

selection_statement : T_IF T_LBRACKET assignment_expression T_RBRACKET statement                   { $$ = new Statement(IF, $3, $5); }
                    | T_IF T_LBRACKET assignment_expression T_RBRACKET statement T_ELSE statement  { $$ = new Statement(ELSE, $3, $5, $7); }
                    | T_SWITCH T_LBRACKET assignment_expression T_RBRACKET statement               { $$ = new Statement(SWITCH, $3, $5); }

iteration_statement : T_WHILE T_LBRACKET assignment_expression T_RBRACKET statement                                         { $$ = new Statement(WHILE, $3, $5); }
                    | T_DO statement T_WHILE T_LBRACKET assignment_expression T_RBRACKET T_SEMICOLON                        { $$ = new Statement(DO, $5, $2); }
                    | T_FOR T_LBRACKET expression_statement expression_statement T_RBRACKET statement                       { $$ = new Statement(FOR, $3, $4, $6); }
                    | T_FOR T_LBRACKET expression_statement expression_statement assignment_expression T_RBRACKET statement { $$ = new Statement(FOR, $3, $4, $5, $7); }

jump_statement : T_CONTINUE T_SEMICOLON                                                            { $$ = new Statement(CONTINUE); }
               | T_BREAK T_SEMICOLON                                                               { $$ = new Statement(BREAK); }
               | T_RETURN T_SEMICOLON                                                              { $$ = new Statement(RETURN); }
               | T_RETURN assignment_expression T_SEMICOLON                                        { $$ = new Statement(RETURN, $2); }

/* Root */
ROOT : translation_unit                                                                            { root = $1; }

translation_unit : external_declaration                                                            { $$ = new TranslationUnit($1); }
                 | translation_unit external_declaration                                           { $$ = new TranslationUnit($1, $2); }

external_declaration : function_definition                                                         { $$ = $1; }
                     | declaration                                                                 { $$ = $1; }

declaration : declaration_specifiers T_SEMICOLON                                                   { $$ = new Declaration($1); }
            | declaration_specifiers init_declarator_list T_SEMICOLON                              { $$ = new Declaration($1, $2); }

declaration_specifiers : T_TYPEDEF                                                                 { $$ = new DeclarationSpecifiers(); }
                       | T_TYPEDEF declaration_specifiers                                          { $$ = new DeclarationSpecifiers($2); }
                       | type_specifier                                                            { $$ = new DeclarationSpecifiers($1); }
                       | type_specifier declaration_specifiers                                     { $$ = new DeclarationSpecifiers($1, $2); }

init_declarator_list : init_declarator                                                             { $$ = new std::vector<init_declarator*>{$1}; }
                     | init_declarator_list T_COMMA init_declarator                                { $1.push_back($3); $$ = $1; }

type_specifier : T_CHAR                                                                            { $$ = new PrimitiveType(T_CHAR); }
               | T_INT                                                                             { $$ = new PrimitiveType(T_INT); }
               | T_FLOAT                                                                           { $$ = new PrimitiveType(T_FLOAT); }
               | T_DOUBLE                                                                          { $$ = new PrimitiveType(T_DOUBLE); }
               | T_UNSIGNED                                                                        { $$ = new PrimitiveType(T_UNSIGNED); }
               | T_TYPEIDENTIFIER                                                                  { $$ = new TypeIDType($1); }
               | struct_specifier                                                                  { $$ = new StructType(*$1); }
               | enum_specifier                                                                    { $$ = new EnumType($1); }

init_declarator : declarator                                                                       { $$ = InitDeclarator($1); }
                | declarator T_ASSIGN initializer                                                  { $$ = InitDeclarator($1, $3); }

initializer : assignment_expression                                                                { $$ = new Initializer($1); }
            | T_CURLY_LBRACKET initializer_list T_CURLY_RBRACKET                                   { $$ = new Initializer($2); }
            | T_CURLY_LBRACKET initializer_list T_COMMA T_CURLY_RBRACKET                           { $$ = new Initializer($2); }

initializer_list : initializer                                                                     { $$ = new std::vector<initializer*>{$1}; }
                 | initializer_list T_COMMA initializer                                            { $1.push_back($3); $$ = $1; }

function_definition : declaration_specifiers declarator compound_statement                         { $$ = new FunctionDefinition($1, $2, $3); }

declarator : pointer direct_declarator                                                             { $$ = new Declarator($1, $2); }
           | direct_declarator                                                                     { $$ = new Declarator($1); }

direct_declarator : T_IDENTIFIER                                                                   { $$ = DirectDeclarator(*$1); }
                  | T_LBRACKET declarator T_RBRACKET                                               { $$ = DirectDeclarator($2); }
                  | direct_declarator T_SQUARE_LBRACKET constant_expression T_SQUARE_RBRACKET      { $$ = DirectDeclarator($1, $3); }
                  | direct_declarator T_SQUARE_LBRACKET T_SQUARE_RBRACKET                          { $$ = DirectDeclarator(SQUARE, $1); }
                  | direct_declarator T_LBRACKET parameter_list T_RBRACKET                         { $$ = DirectDeclarator($1, $3); }
                  | direct_declarator T_LBRACKET identifier_list T_RBRACKET                        { $$ = DirectDeclarator($1, $3); }
                  | direct_declarator T_LBRACKET T_RBRACKET                                        { $$ = DirectDeclarator(ROUND, $1); }

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
