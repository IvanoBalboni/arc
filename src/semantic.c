#include "semantic.h"
#include "ast.h"

int GLOBAL_ADR = 0;
int LOCAL_ADR = 0;

void semantic(ast * p){
  if (p == NULL) return;
  symbole * symb;
  switch(p->type){
    case AST_LIRE:
    break;
    case AST_RETOURNE:
      semantic(p->val->retourne);
    break;
    case AST_NB:
    break;
    case AST_ID:
      //TODO: warning pas affect
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_DECLA, LOCAL);
      if (symb != NULL)
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->id, symb->adr, TS_ID, LOCAL) );
      else
        ErrorSemantic("ID Variable pas declaree");
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
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_DECLA, LOCAL);
      if ( symb == NULL){
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl.id, LOCAL_ADR, TS_DECLA, LOCAL) );
        LOCAL_ADR++;
      }
      else
        ErrorSemantic("Variable deja declaree");
      semantic(p->val->decl.exp);
    break;
    case AST_AFF:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_DECLA, LOCAL);
      if (symb != NULL)
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->affect.id, symb->adr, TS_AFFECT, LOCAL) );
      else
        ErrorSemantic("AFF Variable pas declaree");
      semantic(p->val->affect.exp);
    break;
  }
}


void ErrorSemantic(const char * errmsg){
  fprintf(stderr,"[SEMANTIC error] %s\n",errmsg);
  exit(1);
}
