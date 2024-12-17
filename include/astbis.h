#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  TXT_RED    "\x1b[31m"
#define  TXT_GREEN  "\x1b[32m"
#define  TXT_BLUE   "\x1b[34m"
#define  TXT_BOLD   "\x1b[1m"
#define  TXT_NULL   "\x1b[0m"


enum {AST_NB = 256, AST_OP = 257} ;

typedef union val{
  int         nb;
  struct ast* noeudOP[2];
}val;

typedef struct ast{
  int  type;
  char type_str[32];
  val* valeur;
} ast;

ast * CreerFeuilleNB(int nb);

void FreeAst(ast * p);

void PrintAst(ast * p);
void ErrorAst(const char * errmsg);


#endif
