%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
%}

ALPHABET           [a-zA-Z_]
DIGIT              [0-9]
HEXADECIMAL        [0-9A-Fa-f]
OCTADIGIT          [0-7]

%%
  /* Characters */
[/]               { return (T_DIVIDE); }
[*]               { return (T_MULTIPLY); }
[+]               { return (T_PLUS); }
[-]               { return (T_MINUS); }
[%]               { return (T_MODULO); }

[&]               { return (T_BOOLEAN_AND); }
[|]               { return (T_BOOLEAN_OR); }
[!]               { return (T_BOOLEAN_NOT); }
[\^]              { return (T_BOOLEAN_XOR); }

[(]               { return (T_LBRACKET); }
[)]               { return (T_RBRACKET); }
[[]               { return (T_SQUARE_LBRACKET); }
[]]               { return (T_SQUARE_RBRACKET); }
[{]               { return (T_CURLY_LBRACKET); }
[}]               { return (T_CURLY_RBRACKET); }

[>]               { return (T_GREATER); }
[<]               { return (T_LESS); }
[=]               { return (T_ASSIGN); }

  /* Keywords */
"auto"					  { return (T_AUTO); }
"break"					  { return (T_BREAK); }
"case"					  { return (T_CASE); }
"char"					  { return (T_CHAR); }
"const"					  { return (T_CONST); }
"continue"				{ return (T_CONTINUE); }
"default"				  { return (T_DEFAULT); }
"do"					    { return (T_DO); }
"double"				  { return (T_DOUBLE); }
"else"					  { return (T_ELSE); }
"enum"					  { return (T_ENUM); }
"extern"				  { return (T_EXTERN); }
"float"					  { return (T_FLOAT); }
"for"					    { return (T_FOR); }
"goto"					  { return (T_GOTO); }
"if"					    { return (T_IF); }
"int"					    { return (T_INT); }
"long"					  { return (T_LONG); }
"register"				{ return (T_REGISTER); }
"return"			  	{ return (T_RETURN); }
"short"					  { return (T_SHORT); }
"signed"				  { return (T_SIGNED); }
"sizeof"				  { return (T_SIZEOF); }
"static"				  { return (T_STATIC); }
"struct"				  { return (T_STRUCT); }
"switch"				  { return (T_SWITCH); }
"typedef"         { return (T_TYPEDEF); }
"union"					  { return (T_VOLATILE); }
"unsigned"				{ return (T_UNSIGNED); }
"void"					  { return (T_VOID); }
"volatile"				{ return (T_VOLATILE); }
"while"					  { return (T_WHILE); }

  /* Rules */
[a-z]+            { yylval.string=new std::string(yytext);    
                    return (T_VARIABLE); }

[0-9]+            { yylval.string = new std::string(yytext);    
                    return (T_CONSTANT); }

[ \t\r\n]+		    { /* whitespace */ }
.                 { fprintf(stderr, "Invalid token\n"); exit(1); }

%%
[^\]]*

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
