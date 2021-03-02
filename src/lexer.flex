%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
#include "../include/lexer_hack.hpp"

std::vector<LexerContext> context;
context.push_back(LexerContext());

%}

A                       [a-zA-Z_]
D                       [0-9]
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
[~]               { return (T_BITWISE_NOT); }
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
[!=]              { return (T_NOT_EQUAL); }
[>]               { return (T_GREATER); }
[<]               { return (T_LESS); }
[<=]              { return (T_LESS_EQUAL); }
[>=]              { return (T_GREATER_EQUAL); }

  /* Assignment */
[=]               { return (T_ASSIGN); }
[+=]              { return (T_ADD_ASSIGN); }
[-=]              { return (T_SUB_ASSIGN); }
[*=]              { return (T_MUL_ASSIGN); }
[/=]              { return (T_DIV_ASSIGN); }
[%=]              { return (T_MOD_ASSIGN); }
[&=]              { return (T_AND_ASSIGN); }
[|=]              { return (T_OR_ASSIGN); }
[^=]              { return (T_XOR_ASSIGN); }
[<<=]             { return (T_SHIFT_LEFT_ASSIGN); }
[>>=]             { return (T_SHIFT_RIGHT_ASSIGN); }
[++]              { return (T_INCREMENT); }
[--]              { return (T_DECREMENT); }

  /* Characters */
[(]               { return (T_LBRACKET); }
[)]               { return (T_RBRACKET); }
[[]               { return (T_SQUARE_LBRACKET); }
[]]               { return (T_SQUARE_RBRACKET); }
[{]               { context.push_back(context.back()); return (T_CURLY_LBRACKET); }
[}]               { context.pop_back(); return (T_CURLY_RBRACKET); }
[:]               { return (T_COLON); }
[;]               { context.back().InTypedef=false; return (T_SEMICOLON); }
[.]               { return (T_DOT); }
[,]               { return (T_COMMA); }
[->]              { return (T_ARROW); }

  /* Types */
"int"					    { return (T_INT); }
"double"				  { return (T_DOUBLE); }
"float"					  { return (T_FLOAT); }
"char"					  { return (T_CHAR); }
"unsigned"				{ return (T_UNSIGNED); }

  /* Structures */
"if"					    { return (T_IF); }
"else"					  { return (T_ELSE); }
"do"              { return (T_DO); }
"while"					  { return (T_WHILE); }
"for"					    { return (T_FOR); }
"switch"				  { return (T_SWITCH); }
"case"				    { return (T_CASE); }
"default"				  { return (T_DEFAULT); }
"break"					  { return (T_BREAK); }
"continue"				{ return (T_CONTINUE); }
"return"			  	{ return (T_RETURN); }

  /* Keywords */
"enum"					  { return (T_ENUM); }
"sizeof"				  { return (T_SIZEOF); }
"struct"				  { return (T_STRUCT); }
"typedef"         { context.back().InTypedef=true; return (T_TYPEDEF); }


{A}({A}|{D})*                      { yylval.string=new std::string(yytext); return lexer_hack(yylval.string); }   // IDs and TypeIDs

{D}*\.{D}+([Ee][+-]?{D}+)?[Ff]?    { yylval.string = new std::string(yytext); return (T_FLOAT_CONSTANT); }              // Floating constants
{D}+([Ee][+-]?{D}+)[Ff]?           { yylval.string = new std::string(yytext); return (T_FLOAT_CONSTANT); }

[1-9][\d]*[Uu]?                    { yylval.string = new std::string(yytext); return (T_CONSTANT); }              // Integer constants
[0][OCT]*[Uu]?                     { yylval.string = new std::string(to_string(stoi(yytext, 0, 8))); return (T_CONSTANT); }
[0][Xx]{HEX}+[Uu]?                 { yylval.string = new std::string(to_string(stoi(yytext, 0, 16))); return (T_CONSTANT); }

\'(\\.|[^'\\\n])*\'                { yylval.string = new std::string(yytext); return (T_CHAR_CONSTANT); }              // Character constants

\"(\\.|[^'\\\n])*\"                { yylval.string = new std::string(yytext); return (T_STRING_LITERAL); }        // String literals

[ \t\r\n]+		                     { /* whitespace */ }
.                                  { fprintf(stderr, "Invalid token\n"); exit(1); }

%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}

int lexer_hack(std::string text)   // implements lexer hack
{
  int was_type = T_IDENTIFIER;
  for (int i=0; i<context.back().typeIdentifiers.size(); i++)   // checks if text is ID or TypeID
  {
		if(text == context.back().typeIdentifiers[i])
    {
			was_type = T_TYPEIDENTIFIER;
		}
	}

  if(context.back().InTypedef && was_type==T_IDENTIFIER){
  	context.back().typeIdentifiers.push_back(text);
  }

  return was_type;
}
