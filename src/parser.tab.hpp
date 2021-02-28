/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SRC_PARSER_TAB_HPP_INCLUDED
# define YY_YY_SRC_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 5 "src/parser.y"

#include "ast.hpp"
#include <cassert>
#include <vector>

extern const Node *root; // Definition of variable (to match declaration earlier)

int yylex(void);
void yyerror(const char *);

#line 59 "src/parser.tab.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_ASSIGN = 258,
    T_INCREMENT = 259,
    T_MULTIPLY = 260,
    T_DIVIDE = 261,
    T_PLUS = 262,
    T_MINUS = 263,
    T_MODULO = 264,
    T_LBRACKET = 265,
    T_RBRACKET = 266,
    T_SQUARE_LBRACKET = 267,
    T_SQUARE_RBRACKET = 268,
    T_CURLY_LBRACKET = 269,
    T_CURLY_RBRACKET = 270,
    T_COLON = 271,
    T_SEMICOLON = 272,
    T_COMMA = 273,
    T_GREATER = 274,
    T_GREATER_EQUAL = 275,
    T_LESS = 276,
    T_LESS_EQUAL = 277,
    T_EQUAL = 278,
    T_NOT_EQUAL = 279,
    T_INT = 280,
    T_VOID = 281,
    T_IF = 282,
    T_ELSE = 283,
    T_SWITCH = 284,
    T_WHILE = 285,
    T_FOR = 286,
    T_CONTINUE = 287,
    T_BREAK = 288,
    T_RETURN = 289,
    T_IDENTIFIER = 290,
    T_CONSTANT = 291,
    INC_OP = 292,
    T_LITERAL = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "src/parser.y"

	const Node 		*node;
	Declarator 		*declarator_node;
	Declaration 	*declaration_node;
	Statement 		*statement_node;
	Expression 		*expression_node;
	std::string 	*string;
	int 			int_num;

	std::vector<Expression*>* 	argument_list_vector;
	std::vector<Statement*>* 	statement_list_vector;
	std::vector<Declaration*>* 	declaration_list_vector;
	std::vector<Declarator*>* 	declarator_list_vector;

#line 124 "src/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PARSER_TAB_HPP_INCLUDED  */
