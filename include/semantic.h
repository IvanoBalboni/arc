#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "ts.h"

extern table_symb * TABLE_SYMBOLES;

void semantic(ast * p);

void ErrorSemantic(const char * errmsg);
#endif
