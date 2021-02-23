%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Expression *g_root; // A way of getting the AST out

  // This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

%union{
  const Expression *expr;
  double number;
  std::string *string;
}

%token T_MULTIPLY T_DIVIDE T_PLUS T_MINUS T_MODULO
%token T_BOOLEAN_AND T_BOOLEAN_OR T_BOOLEAN_NOT T_BOOLEAN_XOR 
%token T_LBRACKET T_RBRACKET T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET
%token T_GREATER T_LESS T_ASSIGN
%token T_CHAR T_INT T_LONG T_SIGNED T_FLOAT T_DOUBLE T_VOID T_CONST T_SHORT T_UNSIGNED T_VOLATILE
%token T_CASE T_DEFAULT T_IF T_ELSE T_SWITCH T_WHILE T_DO T_FOR T_GOTO T_CONTINUE T_BREAK T_RETURN
%token T_TYPEDEF T_EXTERN T_STATIC T_AUTO T_REGISTER
%token T_STRUCT T_ENUM T_ELLIPSIS


%start ROOT

%%

ROOT : EXPR { g_root = $1; }

/* TODO-3 : Add support for (x + 6) and (10 - y). You'll need to add production rules, and create an AddOperator or
            SubOperator. */
EXPR : TERM                                               { $$ = $1; }
     | EXPR T_PLUS EXPR                                   { $$ = new AddOperator($1, $3); }
     | EXPR T_MINUS EXPR                                  { $$ = new SubOperator($1, $3); }

/* TODO-4 : Add support (x * 6) and (z / 11). */
TERM : FACTOR                                             { $$ = $1; }
     | TERM T_TIMES TERM                                  { $$ = new MulOperator($1, $3); }
     | TERM T_DIVIDE TERM                                 { $$ = new DivOperator($1, $3); }
     | TERM T_EXPONENT TERM                               { $$ = new ExpOperator($1, $3); }
     | T_MINUS TERM                         %prec UNMINUS { $$ = new NegOperator($2); }

/*  TODO-5 : Add support for (- 5) and (- x). You'll need to add production rules for the unary minus operator and create a NegOperator. */

/* TODO-2 : Add a rule for variable, base on the pattern of number. */
FACTOR : T_NUMBER                                         { /* TODO-1 : uncomment this:  */ $$ = new Number( $1 );  }
       | T_LBRACKET EXPR T_RBRACKET                       { $$ = $2; }
       | T_VARIABLE                                       { $$ = new Variable( *$1 );  }
       | T_LOG T_LBRACKET EXPR T_RBRACKET                 { $$ = new LogFunction( $3 );  }
       | T_EXP T_LBRACKET EXPR T_RBRACKET                 { $$ = new ExpFunction( $3 );  }
       | T_SQRT T_LBRACKET EXPR T_RBRACKET                { $$ = new SqrtFunction( $3 );  }

/* TODO-6 : Add support log(x), by modifying the rule for FACTOR. */

/* TODO-7 : Extend support to other functions. Requires modifications here, and to FACTOR. */
FUNCTION_NAME : T_LOG { $$ = new std::string("log"); }
FUNCTION_NAME : T_EXP { $$ = new std::string("exp"); }
FUNCTION_NAME : T_SQRT { $$ = new std::string("sqrt"); }

%%

const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
