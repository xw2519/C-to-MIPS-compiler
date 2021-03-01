#ifndef STATEMENTS
#define STATEMENTS
#include "ast_node.hpp"

enum StatementEnum { CASE, DEFAULT, EXPRESSION, IF, ELSE, SWITCH, DO, WHILE, FOR, CONTINUE, BREAK, RETURN };

struct Statement
{
  Statement(StatementEnum _type)                                          // Creates empty, continue, break and empty return statements
    : type(_type)
  {}
  Statement(StatementEnum _type, Expression* _expression)                 // creates expression statements and return expression statements
    : type(_type)
  {
    expressions.push_back(_expression);
  }
  Statement(StatementEnum _type, Statement* _branch)                      // creates default statements
    : type(_type)
  {
    branches.push_back(_branch);
  }
  Statement(StatementEnum _type, Expression* _expression, Statement* _branch)                 // creates case, if, switch, while and do statements
    : type(_type)
  {
    expressions.push_back(_expression);
    branches.push_back(_branch);
  }
  Statement(StatementEnum _type, Expression* _expression, Statement* _branch1, Statement* _branch2)     // creates else statements
    : type(_type)
  {
    expressions.push_back(_expression);
    branches.push_back(_branch1);
    branches.push_back(_branch2);
  }
  Statement(StatementEnum _type, std::vector<Expression*> _expressions, Statement* _branch)             // creates for statements
    : type(_type)
    , expressions(_expressions)
  {
    branches.push_back(_branch);
  }

  StatementEnum type;
  std::vector<Expression*> expressions;
  std::vector<Statement*> branch;
};

#endif
