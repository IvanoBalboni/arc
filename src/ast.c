#include "ast.h"
#include "codegen.h"

char * STR_OP[13] = {
  "+" , "-" , "*" , "/", "%",
  "<=", "<" , ">=", ">",
  "=" , "!=", "OU", "ET"
};

static void PrintNB(ast *p, char * indent);
static void PrintID(ast *p, char * indent);
static void PrintOP(ast *p, char * indent);
static void PrintDECLA(ast *p, char * indent);
static void PrintAFFECT(ast *p, char * indent);
static void PrintINST(ast *p, char * indent);
static void PrintLIRE(ast *p, char * indent);
static void PrintRETOURNE(ast *p, char * indent);


ast * CreerFeuilleNB(int nb){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_NB;
  strcpy(p->type_str,"NB");

  valeur* n;
  if ((n = malloc(sizeof(int))) == NULL)
    ErrorAst("echec allocation mémoire");

  p->val = n;
  p->val->nb = nb;

  return p;
}

ast * CreerFeuilleID(char* id){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_ID;
  strcpy(p->type_str,"ID");

  valeur* n;
  if ((n = malloc(sizeof(variable))) == NULL)
    ErrorAst("echec allocation mémoire");

  p->val = n;
  strcpy(p->val->id, id);

  return p;
}

ast * CreerFeuilleLIRE(){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_LIRE;
  strcpy(p->type_str,"LIRE()");

  valeur* n;
  if ((n = malloc(sizeof(int))) == NULL)
    ErrorAst("echec allocation mémoire");

  p->val = n;

  return p;

}


ast * CreerFeuilleRETOURNE(ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_RETOURNE;
  strcpy(p->type_str,"RETOURNE");

  valeur* n;
  if ((n = malloc(sizeof(ast*))) == NULL)
    ErrorAst("echec allocation mémoire");

  p->val = n;
  p->val->retourne = exp;

  return p;

}

ast * CreerFeuilleDECLA(char* id, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_DECL;
  strcpy(p->type_str,"DECL");

  valeur* n;
  if ((n = malloc(sizeof(affectation))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->decl.id, id);
  p->val->decl.exp = exp;

  return p;
}

ast * CreerFeuilleAFFECT(char* id, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_AFF;
  strcpy(p->type_str,"AFFECT");

  valeur* n;
  if ((n = malloc(sizeof(affectation))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->affect.id, id);
  p->val->affect.exp = exp;

  return p;
}


ast * CreerNoeudOP(typeOP op, ast* n1, ast* n2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_OP;
  strcpy(p->type_str, (char[2]){op, '\0'});

  valeur* n;
  if ((n = malloc(sizeof(noeudOP))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;
  p->val->op.type = op;
  p->val->op.val[0] = n1;
  p->val->op.val[1] = n2;

  return p;
}


ast * CreerNoeudINSTRUCT(ast* inst, ast* suiv){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_INST;
  strcpy(p->type_str,"INST");

  valeur* n;
  if ((n = malloc(sizeof(instructions))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  p->val->inst.val  = inst;
  p->val->inst.suiv = suiv;

  return p;
}

ast * CreerNoeudSI(ast* exp, ast* alors,  ast* sinon){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_SI;
  strcpy(p->type_str,"SI");

  valeur* n;
  if ((n = malloc(sizeof(instructions))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  p->val->si.exp = exp;
  p->val->si.alors = alors;
  p->val->si.sinon = sinon;

  return p;
}

ast * CreerNoeudTQ(ast* exp, ast* faire){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_TQ;
  strcpy(p->type_str,"TQ");

  valeur* n;
  if ((n = malloc(sizeof(instructions))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  p->val->tq.exp = exp;
  p->val->tq.faire = faire;

  return p;
}


void FreeAst(ast * p){
  if (p == NULL) return;
  free(p);
}

void PrintAst(ast * p, char* indent){
  if (p == NULL) return;
  switch(p->type){
  case AST_NB:
    PrintNB(p,indent);
    break;
  case AST_ID:
    PrintID(p,indent);
    break;
  case AST_OP:
    PrintOP(p,indent);
    break;
  case AST_AFF:
    PrintAFFECT(p, indent);
    break;
  case AST_DECL:
    PrintDECLA(p, indent);
    break;
  case AST_INST:
    PrintINST(p, indent);
    if(p->val->inst.suiv != NULL)
      PrintAst(p->val->inst.suiv, indent);
    break;
  case AST_SI:
  printf("TODO\n");
    break;
  case AST_TQ:
  printf("TODO\n");
    break;
  case AST_LIRE:
    PrintLIRE(p, indent);
    break;
  case AST_RETOURNE:
    PrintRETOURNE(p, indent);
    break;
  default:
    fprintf(stderr,"[Erreur] type <%d>: %s non reconnu\n",p->type,p->type_str);
    break;
  }
}

void ErrorAst(const char * errmsg){
  fprintf(stderr,"[AST error] %s\n",errmsg);
  exit(1);
}

static void PrintNB(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%d\n",indent, p->val->nb);
}


static void PrintID(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->id);
}

static void PrintOP(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:    " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, STR_OP[p->val->op.type - 512]);
  strcat(indent, "    ");
  PrintAst(p->val->op.val[0], indent);
  PrintAst(p->val->op.val[1], indent);
  indent[strlen(indent) -4] = 0;
}


static void PrintAFFECT(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->affect.id);
  if(p->val->affect.exp == NULL)
    return;
  strcat(indent, "    ");
  PrintAst(p->val->affect.exp, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintDECLA(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->affect.id);
  if(p->val->decl.exp == NULL)
    return;
  strcat(indent, "    ");
  PrintAFFECT(p->val->decl.exp, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintINST(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  strcat(indent, "    ");
  PrintAst(p->val->inst.val, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintLIRE(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
}

static void PrintRETOURNE(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  strcat(indent, "    ");
  PrintAst(p->val->retourne, indent);
  indent[strlen(indent) -4] = 0;
}
