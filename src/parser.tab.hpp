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
#line 12 "src/parser.y"

	#include <cassert>
	#include <vector>

	#include "ast.hpp"

	extern const Node *root; 

	int yylex(void);
	void yyerror(const char *);

#line 60 "src/parser.tab.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_LOGICAL_AND = 258,
    T_LOGICAL_OR = 259,
    T_RIGHT_SHIFT = 260,
    T_LEFT_SHIFT = 261,
    T_ASSIGN = 262,
    T_INCREMENT = 263,
    T_DECREMENT = 264,
    T_ADD_ASSIGN = 265,
    T_SUB_ASSIGN = 266,
    T_MULTIPLY = 267,
    T_DIVIDE = 268,
    T_PLUS = 269,
    T_MINUS = 270,
    T_MODULO = 271,
    T_LBRACKET = 272,
    T_RBRACKET = 273,
    T_SQUARE_LBRACKET = 274,
    T_SQUARE_RBRACKET = 275,
    T_CURLY_LBRACKET = 276,
    T_CURLY_RBRACKET = 277,
    T_COLON = 278,
    T_SEMICOLON = 279,
    T_COMMA = 280,
    T_GREATER = 281,
    T_GREATER_EQUAL = 282,
    T_LESS = 283,
    T_LESS_EQUAL = 284,
    T_EQUAL = 285,
    T_NOT_EQUAL = 286,
    T_INT = 287,
    T_VOID = 288,
    T_CHAR = 289,
    T_DOUBLE = 290,
    T_FLOAT = 291,
    T_UNSIGNED = 292,
    T_SIZEOF = 293,
    T_IF = 294,
    T_ELSE = 295,
    T_SWITCH = 296,
    T_WHILE = 297,
    T_FOR = 298,
    T_CONTINUE = 299,
    T_BREAK = 300,
    T_RETURN = 301,
    T_DEFAULT = 302,
    T_CASE = 303,
    T_IDENTIFIER = 304,
    T_INT_CONSTANT = 305,
    T_FLOAT_CONSTANT = 306,
    T_DOUBLE_CONSTANT = 307,
    T_UNSIGNED_CONSTANT = 308,
    T_BITWISE_AND = 309,
    T_BITWISE_OR = 310,
    T_BITWISE_XOR = 311,
    T_ENUM = 312
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 25 "src/parser.y"

	const Node 		*node;
	Declarator 		*declarator_node;
	Declaration 	*declaration_node;
	Statement 		*statement_node;
	Expression 		*expression_node;
	std::string 	*string;
	type_definition *type_node;
	int 			int_num;
	unsigned int 	unsigned_num;
	float			float_num;
	double 			double_num;

	std::vector<Expression*>* 	argument_list_vector;
	std::vector<Statement*>* 	statement_list_vector;
	std::vector<Declaration*>* 	declaration_list_vector;
	std::vector<Declarator*>* 	declarator_list_vector;

	std::vector<Expression*>* 	initialisation_list;

#line 150 "src/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PARSER_TAB_HPP_INCLUDED  */
