%option noyywrap

/* Definition section */
%{
/* 
   Now in a section of C that will be embedded
   into the auto-generated code. Flex will not
   try to interpret code surrounded by %{ ... %} 

   Bring in our declarations for token types and
   the yylval variable. 
*/
#include "histogram.hpp"
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

// This is to work around an irritating bug in Flex.
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex

// "c_str" used throughout to convert string into C-style string format
extern "C" int fileno(FILE *stream);
%}

A                       [a-zA-Z_]
DIGIT                   [0-9]
HEX                     [0-9A-Fa-f]
OCT                     [0-7]

%%
  /* Arithmetic Operators */
[/]               { return (T_DIVIDE); }
[*]               { return (T_MULTIPLY); }
[+]               { return (T_PLUS); }
[-]               { return (T_MINUS); }
[%]               { return (T_MODULO); }

  /* Bitwise Operators */
[&]               { return (T_BITWISE_AND); }
[|]               { return (T_BITWISE_OR); }
[\^]              { return (T_BITWISE_XOR); }
[<<]              { return (T_BITWISE_SHIFT_LEFT); }
[>>]              { return (T_BITWISE_SHIFT_RIGHT); }

  /* Logical Operators */
[!]               { return (T_LOGICAL_NOT); }
[&&]              { return (T_LOGICAL_AND); }
[||]              { return (T_LOGICAL_OR); }

  /* Comparison */
[==]              { return (T_EQUAL); }
[>]               { return (T_GREATER); }
[<]               { return (T_LESS); }

  /* Assignment */
[=]               { return (T_ASSIGN); }

  /* Characters */
[(]               { return (T_LBRACKET); }
[)]               { return (T_RBRACKET); }
[[]               { return (T_SQUARE_LBRACKET); }
[]]               { return (T_SQUARE_RBRACKET); }
[{]               { return (T_CURLY_LBRACKET); }
[}]               { return (T_CURLY_RBRACKET); }
[:]               { return (T_COLON); }
[;]               { return (T_SEMICOLON); }

  /* Types */
"int"					    { return (T_INT); }
"double"				  { return (T_DOUBLE); }
"float"					  { return (T_FLOAT); }
"char"					  { return (T_CHAR); }
"unsigned"				{ return (T_UNSIGNED); }

  /* Structures */
"if"					    { return (T_IF); }
"else"					  { return (T_ELSE); }
"while"					  { return (T_WHILE); }
"for"					    { return (T_FOR); }
"switch"				  { return (T_SWITCH); }
"break"					  { return (T_BREAK); }
"continue"				{ return (T_CONTINUE); }
"return"			  	{ return (T_RETURN); }

  /* Keywords */
"enum"					  { return (T_ENUM); }
"sizeof"				  { return (T_SIZEOF); }
"struct"				  { return (T_STRUCT); }
"typedef"         { return (T_TYPEDEF); }


{A}({A}|{DIGIT})* { yylval.string=new std::string(yytext);
                    return (T_IDENTIFIER); }

[DIGIT]+          { yylval.string = new std::string(yytext);
                    return (T_CONSTANT); }

[ \t\r\n]+		    { /* whitespace */ }
.                 { fprintf(stderr, "Invalid token\n"); exit(1); }

%%


/* Error handler. This will get called if none of the rules match. */
void yyerror (char const *s)
{
  fprintf (stderr, "Flex Error: %s\n", s); /* s is the text that wasn't matched */
  exit(1);
}
