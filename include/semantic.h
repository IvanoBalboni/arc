#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "ts.h"

extern table_symb * TABLE_SYMBOLES;
extern char         CONTEXTE[32];

void semantic(ast * p);
void semanticDECL(ast * p);
void semanticDECLIDL(ast * p);
void semanticAFF(ast * p);
void semanticSI(ast * p);
void semanticFCT(ast * p);
void semanticAPPL(ast * p);

int lenOP(typeOP op, int exp1, int exp2);

void ErrorSemantic(const char * errmsg);
#endif
