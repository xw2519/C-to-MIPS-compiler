  /*
  Contents and terms are derived from the ANSI C grammer specification
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
  /* Comments (To be done at a later date) */
  // "/*"			{ comment(); }

  /* Types */
"int"				                        { return (T_INT); }
"void"							                { return (T_VOID); }


  /* Structures */
"if"					                      { return (T_IF); }
"else"					                    { return (T_ELSE); }
"while"					                    { return (T_WHILE); }
"for"					                      { return (T_FOR); }
"switch"				                    { return (T_SWITCH); }
"break"					                    { return (T_BREAK); }
"continue"				                  { return (T_CONTINUE); }
"return"			  	                  { return (T_RETURN); }
"default"                           { return (T_DEFAULT); }
"case"                              { return (T_CASE); }


  /* Characters */
[(]                                 { return (T_LBRACKET); }
[)]                                 { return (T_RBRACKET); }
[[]                                 { return (T_SQUARE_LBRACKET); }
[]]                                 { return (T_SQUARE_RBRACKET); }
[{]                                 { return (T_CURLY_LBRACKET); }
[}]                                 { return (T_CURLY_RBRACKET); }
[:]                                 { return (T_COLON); }
[;]                                 { return (T_SEMICOLON); }
[,]                                 { return (T_COMMA); }


  /* Arithmetic Operators */
[/]                                 { return (T_DIVIDE); }
[*]                                 { return (T_MULTIPLY); }
[+]                                 { return (T_PLUS); }
[-]                                 { return (T_MINUS); }
[%]                                 { return (T_MODULO); }


  /* Comparison */
[=][=]                              { return (T_EQUAL); }
[>]                                 { return (T_GREATER); }
[<]                                 { return (T_LESS); }
[<][=]				                      { return (T_LESS_EQUAL); }
[>][=]				                      { return (T_GREATER_EQUAL); }
[!][=]				                      { return (T_NOT_EQUAL);	}


  /* Assignment */
[=]                                 { return (T_ASSIGN); }
[+][+]				                      { return (T_INCREMENT); }
[-][-]				                      { return (T_DECREMENT); }

  /* Bitwise */
[&]                                 { return (T_BITWISE_AND); }
[|]                                 { return (T_BITWISE_OR); }
[\^]                                { return (T_BITWISE_XOR); }

  /* Logical */
[&][&]                              { return (T_LOGICAL_AND); }
[|][|]                              { return (T_LOGICAL_OR); }


  /* Other */
{L}({L}|{D})*                       { yylval.string  = new std::string(yytext); return (T_IDENTIFIER); }
{D}+							                  { yylval.int_num = strtod(yytext, 0);       return (T_CONSTANT); }
[L]?["](\\.|[^\\"\n])*["]		        { yylval.string  = new std::string(yytext);	return (T_LITERAL); }
[ \t\r\n]+		                      { ; }
.                                   { fprintf(stderr, "Invalid token\n"); exit(1); }

%%


void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}

