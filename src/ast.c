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
static void PrintSI(ast *p, char * indent);
static void PrintTQ(ast *p, char * indent);
static void PrintFCT(ast *p, char * indent);
static void PrintAPPEL_FCT(ast *p, char * indent);
static void PrintLIST(ast *p, char * indent);
static void PrintIDL(ast *p, char * indent);
static void PrintAFF_IDL(ast *p, char * indent);
static void PrintDECL_IDL(ast *p, char * indent);

static void PrintLIRE(ast *p, char * indent);
static void PrintECRIRE(ast *p, char * indent);
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

ast * CreerFeuilleECRIRE(ast * exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_ECRIRE;
  strcpy(p->type_str,"ECRIRE");

  valeur* n;
  if ((n = malloc(sizeof(ast*))) == NULL)
    ErrorAst("echec allocation mémoire");

  p->val = n;
  p->val->ecrire = exp;

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
  if(exp == NULL)
    return p;
    
  ast * aff = CreerFeuilleAFFECT(id, exp);
  p->val->decl.exp = aff;

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


ast * CreerFeuilleDECLALISTE(char* id, int taille){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_DECL_IDL;
  strcpy(p->type_str,"DECL_LISTE");

  valeur* n;
  if ((n = malloc(sizeof(decla_liste))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->decl_liste.id, id);
  p->val->decl_liste.taille = taille;

  return p;
}

ast * CreerNoeudLISTE(ast * exp, ast * suiv){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_LIST;
  strcpy(p->type_str,"LISTE");

  valeur* n;
  if ((n = malloc(sizeof(instructions))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  p->val->liste.val = exp;
  p->val->liste.suiv = suiv;

  return p;
}


ast * CreerFeuilleLIRE_ELEM_LISTE(char* id, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_IDL;
  strcpy(p->type_str,"ELEM_LISTE");

  valeur* n;
  if ((n = malloc(sizeof(affectation))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->elem_liste.id, id);
  p->val->elem_liste.exp = exp;

  return p;
}

ast * CreerFeuilleAFFECTLISTE(char* id, ast* pos, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_AFF_IDL;
  strcpy(p->type_str,"AFF_LISTE");

  valeur* n;
  if ((n = malloc(sizeof(affect_elem_liste))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->affect_liste.id, id);
  p->val->affect_liste.pos = pos;
  p->val->affect_liste.exp = exp;

  return p;
}


ast * CreerNoeudOP(typeOP op, ast* n1, ast* n2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_OP;
  strcpy(p->type_str, "OP");

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


ast * CreerFONCTION(char* id, ast* param, ast* decla, ast* inst){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_FCT;
  strcpy(p->type_str,"FONCTION");

  valeur* n;
  if ((n = malloc(sizeof(decl_fonction))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->algo.id, id);
  p->val->algo.param = param;
  p->val->algo.decl_liste = decla;
  p->val->algo.inst = inst;

  return p;
}

ast * CreerAPPEL_FONCTION(char* id, ast* param){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_APPEL_FCT;
  strcpy(p->type_str,"APPEL_FONCTION");

  valeur* n;
  if ((n = malloc(sizeof(affectation))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->appel.id, id);
  //TODO: check liste param bonne taille?
  p->val->appel.exp = param;

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
    PrintSI(p, indent);
    break;
  case AST_TQ:
    PrintTQ(p, indent);
    break;
  case AST_FCT:
    PrintFCT(p, indent);
    break;
  case AST_APPEL_FCT:
    PrintAPPEL_FCT(p, indent);
    break;
  case AST_LIST:
    PrintLIST(p, indent);
  break;
  case AST_IDL:
    PrintIDL(p, indent);
    break;
  case AST_AFF_IDL:
    PrintAFF_IDL(p, indent);
    break;
  case AST_DECL_IDL:
    PrintDECL_IDL(p, indent);
    break;
  case AST_LIRE:
    PrintLIRE(p, indent);
    break;
  case AST_ECRIRE:
    PrintECRIRE(p, indent);
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

static void PrintSI(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  //SI EXP
  strcat(indent, "    ");
  PrintAst(p->val->si.exp, indent);
  indent[strlen(indent) -4] = 0;

  //ALORS INST
  strcat(indent, "    ");
  PrintAst(p->val->si.alors, indent);
  indent[strlen(indent) -4] = 0;

  //SINON INST
  strcat(indent, "    ");
  PrintAst(p->val->si.sinon, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintTQ(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  //TQ EXP
  strcat(indent, "    ");
  PrintAst(p->val->tq.exp, indent);
  indent[strlen(indent) -4] = 0;

  // INST
  strcat(indent, "    ");
  PrintAst(p->val->tq.faire, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintFCT(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->algo.id);
  //PARAM
  strcat(indent, "    ");
  PrintAst(p->val->algo.param, indent);
  indent[strlen(indent) -4] = 0;

  //DECLA
  strcat(indent, "    ");
  PrintAst(p->val->algo.decl_liste, indent);
  indent[strlen(indent) -4] = 0;

  //INST
  strcat(indent, "    ");
  PrintAst(p->val->algo.inst, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintAPPEL_FCT(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->appel.id);

  //param a passer
  strcat(indent, "    ");
  PrintAst(p->val->appel.exp, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintLIST(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  strcat(indent, "    ");
  ast * exp = p;
  while(exp != NULL){
    PrintAst(exp->val->liste.val, indent);
    exp = exp->val->liste.suiv;
  }
  indent[strlen(indent) -4] = 0;

}

static void PrintIDL(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->elem_liste.id);

  strcat(indent, "    ");
  PrintAst(p->val->elem_liste.exp, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintAFF_IDL(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->affect_liste.id);

  //POS
  strcat(indent, "    ");
  PrintAst(p->val->affect_liste.pos, indent);
  indent[strlen(indent) -4] = 0;

  //EXP
  strcat(indent, "    ");
  PrintAst(p->val->affect_liste.exp, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintDECL_IDL(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->decl_liste.id);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%d\n",indent, p->val->decl_liste.taille);
  
}

static void PrintLIRE(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
}

static void PrintECRIRE(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  strcat(indent, "    ");
  PrintAst(p->val->ecrire, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintRETOURNE(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  strcat(indent, "    ");
  PrintAst(p->val->retourne, indent);
  indent[strlen(indent) -4] = 0;
}
