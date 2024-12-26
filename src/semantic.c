#include "semantic.h"
#include "ast.h"

int LOCAL_ADR = 0;

void semantic(ast * p){
  if (p == NULL) return;
  symbole * symb;
  switch(p->type){
    case AST_LIRE:
    break;
    case AST_ECRIRE:
      semantic(p->val->ecrire);
    break;
    case AST_RETOURNE:
      semantic(p->val->retourne);
    break;
    case AST_NB:
    break;
    case AST_ID:
      //TODO: warning pas affect ?
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_AFFECT);
      if (symb == NULL)
        ErrorSemantic("ID Variable pas declaree");
      //AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->id, symb->adr, TS_ID));
    break;
    case AST_OP:
      semantic(p->val->op.val[0]);
      semantic(p->val->op.val[1]);
    break;
    case AST_INST:
      semantic(p->val->inst.val);
      if (p->val->inst.suiv != NULL)
        semantic(p->val->inst.suiv);
    break;
    case AST_DECL:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_DECLA);
      if ( symb == NULL){
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl.id, LOCAL_ADR, TS_DECLA));
        LOCAL_ADR++;
      }
      else
        ErrorSemantic("Variable deja declaree");
      semantic(p->val->decl.exp);
    break;
    case AST_AFF:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_DECLA);
      if (symb == NULL)
        ErrorSemantic("AFF Variable pas declaree");
      else{
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(symb->id, symb->adr, TS_AFFECT));
      }
      semantic(p->val->affect.exp);
    break;
    case AST_SI:
    break;
    case AST_TQ:
    break;
    case AST_FCT:
    break;
    case AST_APPEL_FCT:
    break;
    case AST_LIST:
    break;
    case AST_IDL:
    break;
    case AST_DECL_IDL:
    break;
    case AST_AFF_IDL:
    break;
  }
}

void semanticContexte(char* ctxt){
  strcpy(CONTEXTE, ctxt);
}


void ErrorSemantic(const char * errmsg){
  fprintf(stderr,"[SEMANTIC error] %s\n",errmsg);
  exit(1);
}
