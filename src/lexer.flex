  /*

  Contents and terms are derived from the ANSI C grammer specification (ANSI/ISO 9899/1990) specification
  
  https://www.lysator.liu.se/c/ANSI-C-grammar-l.html
  
  */

%option noyywrap

%{
  #include "parser.tab.hpp"
%}


D			  [0-9]
L			  [a-zA-Z_]
H			  [a-fA-F0-9]
E			  [Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*


%%

 /* --------------- 			  			        Types			 				        --------------- */
"int"				                        { return (T_INT); }
"void"							                { return (T_VOID); }
"char"                              { return (T_CHAR); }
"double"                            { return (T_DOUBLE); }
"float"                             { return (T_FLOAT); }
"unsigned"                          { return (T_UNSIGNED); }

 /* --------------- 			  			Conditional Structure			 				--------------- */
"if"					                      { return (T_IF); }
"else"					                    { return (T_ELSE); }

 /* --------------- 			  			   Loop Structure			 				    --------------- */
"while"					                    { return (T_WHILE); }
"for"					                      { return (T_FOR); }
"break"					                    { return (T_BREAK); }
"continue"				                  { return (T_CONTINUE); }
"return"			  	                  { return (T_RETURN); }

 /* --------------- 			  			  Switch Structure			 				  --------------- */
"switch"				                    { return (T_SWITCH); }
"case"                              { return (T_CASE); }
"default"                           { return (T_DEFAULT); }

 /* --------------- 			  			  Sizeof Structure			 				  --------------- */
"sizeof"                            { return (T_SIZEOF); }

  /* --------------- 			  			Enumerator Structure			 				--------------- */
"enum"                              { return (T_ENUM); }

 /* --------------- 			  			     Characters			 				      --------------- */
[(]                                 { return (T_LBRACKET); }
[)]                                 { return (T_RBRACKET); }
[[]                                 { return (T_SQUARE_LBRACKET); }
[]]                                 { return (T_SQUARE_RBRACKET); }
[{]                                 { return (T_CURLY_LBRACKET); }
[}]                                 { return (T_CURLY_RBRACKET); }
[:]                                 { return (T_COLON); }
[;]                                 { return (T_SEMICOLON); }
[,]                                 { return (T_COMMA); }

 /* --------------- 			  			 Arithmetic Operators			 				--------------- */
[/]                                 { return (T_DIVIDE); }
[*]                                 { return (T_MULTIPLY); }
[+]                                 { return (T_PLUS); }
[-]                                 { return (T_MINUS); }
[%]                                 { return (T_MODULO); }

 /* --------------- 			  			      Comparison			 				    --------------- */
[=][=]                              { return (T_EQUAL); }
[>]                                 { return (T_GREATER); }
[<]                                 { return (T_LESS); }
[<][=]				                      { return (T_LESS_EQUAL); }
[>][=]				                      { return (T_GREATER_EQUAL); }
[!][=]				                      { return (T_NOT_EQUAL);	}

 /* --------------- 			  			      Assignment			 				    --------------- */
[=]                                 { return (T_ASSIGN); }
[+][+]				                      { return (T_INCREMENT); }
[-][-]				                      { return (T_DECREMENT); }
[+][=]				                      { return (T_ADD_ASSIGN); }
[-][=]				                      { return (T_SUB_ASSIGN); }

 /* --------------- 			  			       Bitwise			 				      --------------- */
[&]                                 { return (T_BITWISE_AND); }
[|]                                 { return (T_BITWISE_OR); }
[\^]                                { return (T_BITWISE_XOR); }

 /* --------------- 			  			       Logical			 				      --------------- */
[&][&]                              { return (T_LOGICAL_AND); }
[|][|]                              { return (T_LOGICAL_OR); }
[>][>]                              { return (T_RIGHT_SHIFT); }
[<][<]                              { return (T_LEFT_SHIFT); }

 /* --------------- 			  		 Integer, Float, Double             --------------- */

{D}+							                  { yylval.int_num   = strtod(yytext, 0);     return (T_INT_CONSTANT); }
[0-9]+[.][0-9]+[f|F|l|L]            { yylval.float_num = strtod(yytext, 0);     return (T_FLOAT_CONSTANT); }
[0-9]+[.][0-9]+                     { yylval.float_num = strtod(yytext, 0);     return (T_DOUBLE_CONSTANT); }
[0-9]+[.][0-9]+[u|U]                { yylval.float_num = strtod(yytext, 0);     return (T_UNSIGNED_CONSTANT); }

 /* --------------- 			  			        Other			 				        --------------- */
{L}({L}|{D})*                       { yylval.string  = new std::string(yytext); return (T_IDENTIFIER); }

[ \t\r\n]+		                      { ; }
.                                   { fprintf(stderr, "Invalid token\n");       exit(1); }



%%


void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}

