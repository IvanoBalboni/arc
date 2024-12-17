#ifndef CODEGEN_H
#define CODEGEN_H

#include "ts.h"

#define NB_REGISTRES 9

typedef enum CONTEXT{GLOBAL_VAR = 128, LOCAL_VAR = 129, FUNCTION = 130}CONTEXT;

#define EMPILER() PILE++    \

#define DEPILER() PILE--    \


int chercher(table_symb ts, CONTEXT);

//utilise la TS pour initialiser PILE apres les var globales~
void codegen_init();
#endif