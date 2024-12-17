#include "ast.h"

static void PrintNB(ast *p, char * indent);
static void PrintOP(ast *p, char * indent);


ast * CreerFeuilleNB(int nb){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_NB;
  strcpy(p->type_str,"NB");

  val* n;
  if ((n = malloc(sizeof(int))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->valeur = n;
  p->valeur->nb = nb;

  return p;
}


ast * CreerFeuilleDECLA(char* id){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_NB;
  strcpy(p->type_str,"NB");

  val* n;
  if ((n = malloc(sizeof(variable))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->valeur = n;
  strcpy(p->valeur->var.id, id);
  p->valeur->var.nb = 0;

  return p;
}

ast * CreerFeuilleAFFECT(char* id, int nb){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_NB;
  strcpy(p->type_str,"NB");

  val* n;
  if ((n = malloc(sizeof(variable))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->valeur = n;
  strcpy(p->valeur->var.id, id);
  p->valeur->var.nb = nb;

  return p;
}

ast * CreerNoeudOP(char op, ast* n1, ast* n2){
  printf("noeud\n" );
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_OP;
  strcpy(p->type_str, &op);

  val* n;
  if ((n = malloc(sizeof(noeudOP))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->valeur = n;
  p->valeur->op[0] = n1;
  p->valeur->op[1] = n2;

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
  case AST_OP:
    PrintOP(p,indent);
    int len = strlen(indent);
    strcat(indent, "    ");
    PrintAst(p->valeur->op[0], indent);
    PrintAst(p->valeur->op[1], indent);
    indent[len -4] = 0;
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
  printf("%s" TXT_BOLD "Valeur: " TXT_NULL "%d\n",indent, p->valeur->nb);
}

static void PrintOP(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Ope:    " TXT_NULL "%s\n",indent, p->type_str);
}
