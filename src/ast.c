#include "ast.h"
#include "codegen.h"

char * STR_OP[13] = {
  "+" , "-" , "*" , "/", "%%",
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

ast * CreerFeuilleADR(char* id){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_ADR;
  strcpy(p->type_str,"ADR");

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


ast * CreerFeuilleDECLA_IDL(char* id, int taille, ast * liste){
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
  p->val->decl_liste.liste = liste;

  return p;
}

ast * CreerNoeudLISTE(ast * exp, ast * suiv){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_LIST;
  strcpy(p->type_str,"LISTE");

  valeur* n;
  if ((n = malloc(sizeof(Liste))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  if(suiv != NULL)
    p->val->liste.taille = suiv->val->liste.taille +1;
  else
    p->val->liste.taille = 1;
  
  p->val->liste.val = exp;
  p->val->liste.suiv = suiv;

  return p;
}


ast * CreerFeuilleLIRE_IDL(char* id, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_IDL;
  strcpy(p->type_str,"IDL");

  valeur* n;
  if ((n = malloc(sizeof(affectation))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->IDL.id, id);
  p->val->IDL.exp = exp;

  return p;
}

ast * CreerFeuilleAFFECT_IDL(char* id, ast* pos, ast* exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_AFF_IDL;
  strcpy(p->type_str,"AFF_IDL");

  valeur* n;
  if ((n = malloc(sizeof(affect_IDL))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  strcpy(p->val->affect_idl.id, id);
  p->val->affect_idl.pos = pos;
  p->val->affect_idl.exp = exp;

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
  //printf("str dans fct %s\n",id);
  p->val->algo.param = param;
  if(inst == NULL)
    return p;
  p->val->algo.decl_liste = decla;
  p->val->algo.inst = inst;

  return p;
}


ast * CreerNoeudPROGRAMME(ast* pre_main, ast* main){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_PROGRAMME;
  strcpy(p->type_str,"PROGRAMME");

  valeur* n;
  if ((n = malloc(sizeof(programme))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  if (pre_main != NULL)
    p->val->prog.nb_global = pre_main->val->pre_main.nb_global;
  else
    p->val->prog.nb_global = 0;
  
  p->val->prog.pre_main = pre_main;
  p->val->prog.main = main;

  return p;
}

ast * CreerNoeudPRE_MAIN(ast* decla, ast* suiv){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_PRE_MAIN;
  strcpy(p->type_str,"PRE_MAIN");

  valeur* n;
  if ((n = malloc(sizeof(programme))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  if(suiv != NULL)
    p->val->pre_main.nb_global = suiv->val->pre_main.nb_global +1;
  else
    p->val->pre_main.nb_global = 1;
  
  p->val->pre_main.pre_main = decla;
  p->val->pre_main.main = suiv;

  return p;


}

ast * CreerNoeudMAIN(ast* decla, ast* inst){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_MAIN;
  strcpy(p->type_str,"MAIN");

  valeur* n;
  if ((n = malloc(sizeof(instructions))) == NULL)
    ErrorAst("echec allocation mémoire");
  p->val = n;

  p->val->main.val = decla;
  p->val->main.suiv = inst;

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
  switch(p->type){
  case AST_NB:
  case AST_ID:
  case AST_ADR:
    break;
  case AST_OP:
    FreeAst(p->val->op.val[0]);
    FreeAst(p->val->op.val[1]);
    break;
  case AST_AFF:
    FreeAst(p->val->affect.exp);
    break;
  case AST_DECL:
    FreeAst(p->val->decl.exp);
    break;
  case AST_INST:
    FreeAst(p->val->inst.val);
    FreeAst(p->val->inst.suiv);
    break;
  case AST_SI:
    FreeAst(p->val->si.exp);
    FreeAst(p->val->si.alors);
    FreeAst(p->val->si.sinon);
    break;
  case AST_TQ:
    FreeAst(p->val->tq.exp);
    FreeAst(p->val->tq.faire);
    break;
  case AST_FCT:
    FreeAst(p->val->algo.param);
    FreeAst(p->val->algo.decl_liste);
    FreeAst(p->val->algo.inst);
    break;
  case AST_APPEL_FCT:
    FreeAst(p->val->appel.exp);
    break;
  case AST_LIST:
    FreeAst(p->val->liste.val);
    FreeAst(p->val->liste.suiv);
  break;
  case AST_IDL:
    FreeAst(p->val->IDL.exp);
    break;
  case AST_AFF_IDL:
    FreeAst(p->val->affect_idl.exp);
    FreeAst(p->val->affect_idl.pos);
    break;
  case AST_DECL_IDL:
    FreeAst(p->val->decl_liste.liste);
    break;
  case AST_LIRE:
    break;
  case AST_ECRIRE:
    FreeAst(p->val->ecrire);
    break;
  case AST_RETOURNE:
    FreeAst(p->val->retourne);
    break;
  case AST_PROGRAMME:
    FreeAst(p->val->prog.pre_main);
    FreeAst(p->val->prog.main);
  case AST_PRE_MAIN:
    FreeAst(p->val->pre_main.pre_main);
    FreeAst(p->val->pre_main.main);
    break;
  case AST_MAIN:
    FreeAst(p->val->main.val);
    FreeAst(p->val->main.suiv);
    break;
  default:
    //fprintf(stderr,"[Erreur] Free type <%d>: %s non reconnu\n",p->type,p->type_str);
    break;
  }
  free(p->val);
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
  case AST_ADR:
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
  case AST_PROGRAMME:
    printf( TXT_BOLD TXT_RED "PROGRAMME :\n");
    PrintAst(p->val->prog.pre_main, indent);
    PrintAst(p->val->prog.main, indent);
    break;
  case AST_PRE_MAIN:
    PrintAst(p->val->pre_main.pre_main, indent);
    if(p->val->pre_main.main != NULL)
      PrintAst(p->val->pre_main.main, indent);
    break;
  case AST_MAIN:
    printf( TXT_BOLD TXT_RED "MAIN :\n");
    printf(TXT_BOLD TXT_RED "arbre declarations\n");
    PrintAst(p->val->main.val, indent);
    printf(TXT_BOLD TXT_RED "arbre instructions\n");
    PrintAst(p->val->main.suiv, indent);
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
  printf("%s" TXT_BOLD TXT_GREEN "CONDITION:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->si.exp, indent);
  indent[strlen(indent) -4] = 0;

  //ALORS INST
  printf("%s" TXT_BOLD TXT_GREEN "ALORS:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->si.alors, indent);
  indent[strlen(indent) -4] = 0;

  //SINON INST
  printf("%s" TXT_BOLD TXT_GREEN "SINON:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->si.sinon, indent);
  indent[strlen(indent) -4] = 0;
}

static void PrintTQ(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);

  //TQ EXP
  printf("%s" TXT_BOLD TXT_GREEN "CONDITION:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->tq.exp, indent);
  indent[strlen(indent) -4] = 0;

  // INST
  printf("%s" TXT_BOLD TXT_GREEN "FAIRE:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->tq.faire, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintFCT(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->algo.id);
  //PARAM
  printf("%s" TXT_BOLD TXT_GREEN "PARAMETRES:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->algo.param, indent);
  indent[strlen(indent) -4] = 0;

  //DECLA
  printf("%s" TXT_BOLD TXT_GREEN "DECLARATIONS:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->algo.decl_liste, indent);
  indent[strlen(indent) -4] = 0;

  //INST
  printf("%s" TXT_BOLD TXT_GREEN "INSTRUCTIONS:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->algo.inst, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintAPPEL_FCT(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->appel.id);

  //param a passer
  printf("%s" TXT_BOLD TXT_GREEN "PARAMETRES:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->appel.exp, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintLIST(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Taille: " TXT_NULL "%d\n",indent, p->val->liste.taille);
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
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->IDL.id);

  strcat(indent, "    ");
  PrintAst(p->val->IDL.exp, indent);
  indent[strlen(indent) -4] = 0;

}

static void PrintAFF_IDL(ast *p, char * indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "val: " TXT_NULL "%s\n",indent, p->val->affect_idl.id);

  //POS
  printf("%s" TXT_BOLD TXT_GREEN "POSITION:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->affect_idl.pos, indent);
  indent[strlen(indent) -4] = 0;

  //EXP
  printf("%s" TXT_BOLD TXT_GREEN "AFFECTER:\n",indent);
  strcat(indent, "    ");
  PrintAst(p->val->affect_idl.exp, indent);
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
