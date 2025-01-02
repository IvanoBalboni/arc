#include "ts.h"
#include "ast.h"

char CONTEXTE[32] = "GLOBAL";
char CHERCHE_SYMB_GLOBAL = 0;// 0 si local, 1 si global

char * STR_TYPE[11] = {
  "ID", "ID_DECLA", "ID_AFFECT",
  "IDL", "IDL_DECLA", "IDL_AFFECT",
  "FCT", "FCT_PROTO", "FCT_DECLA ", "ADR",
  "FIN"
};

table_symb * TableSymbInit(){
  table_symb * ts;
  if ((ts = malloc(sizeof(table_symb))) == NULL)
    ErrorTs("ts echec allocation mémoire");


  strcpy(ts->contexte, "GLOBAL");
  ts->table_ctxt = NULL;
  if ((ts->suiv = malloc(sizeof(table_symb))) == NULL)
    ErrorTs("ts echec allocation mémoire");

  strcpy(ts->suiv->contexte, "MAIN");
  ts->suiv->table_ctxt = NULL;
  ts->suiv->suiv       = NULL;

  return ts;
}

contexte * contexteAdd(symbole * s){
  contexte * tc;
  if ((tc = malloc(sizeof(contexte))) == NULL)
    ErrorTs("Contexte echec allocation mémoire");
  tc->symb = s;
  tc->suiv = NULL;
  return tc;
}

void AjouterCTXT(table_symb * ts, char* ctxt){
  table_symb * suiv = ts;
  while (suiv->suiv != NULL) {
    if (strcmp(suiv->suiv->contexte, ctxt) == 0)
      return; // contexte existe deja
    suiv = suiv->suiv;
  }

  if ((suiv->suiv = malloc(sizeof(table_symb))) == NULL)
    ErrorTs("ctxt echec allocation mémoire");

  strcpy(suiv->suiv->contexte, ctxt);
  suiv->suiv->table_ctxt = NULL;
  suiv->suiv->suiv = NULL;

}

void AjouterSymb(table_symb * ts, symbole * symb){
  table_symb * suiv = ts;
  contexte * ctxt_suiv;
  while (suiv != NULL) {
    if (strcmp(suiv->contexte, CONTEXTE) == 0){
      if ( suiv->table_ctxt == NULL){
        suiv->table_ctxt = contexteAdd(symb);
        return;
      }
      ctxt_suiv = suiv->table_ctxt;
      while (ctxt_suiv->suiv != NULL) { ctxt_suiv= ctxt_suiv->suiv; }
      ctxt_suiv->suiv = contexteAdd(symb);
      return;
    }
    suiv = suiv->suiv;
  }
  ErrorTs("ajout symbole dans un contexte non existant");
}

symbole * CreerSymb(char* id, int adr, Type type){
  symbole * s;
  if ((s = malloc(sizeof(symbole))) == NULL)
    ErrorTs("symbole echec allocation mémoire");
  strcpy(s->id, id);
  s->adr = adr;
  s->type = type;
  return s;
}

symbole * RechercherSymb(table_symb * ts, char * id, Type type){
  /*
    En cas de recherche dans GLOBAL si global n'a pas le symbole
    GLOBAL sera parcouru 2 fois
  */

  if (ts == NULL)
    return NULL;
  table_symb * suiv = ts;
  contexte * ctxt_suiv;
  if (strcmp(CONTEXTE, "GLOBAL") == 0)//recherche dans GLOBAL
    suiv = NULL;
  while (suiv != NULL) { //Parcours du tableau de contextes
    if (strcmp(suiv->contexte, CONTEXTE) == 0){ //contexte trouve
      ctxt_suiv = suiv->table_ctxt;
      while (ctxt_suiv != NULL) { //Parcours du contexte
        symbole * symb = ctxt_suiv->symb;
        if( symb == NULL) break;
        if( type == symb->type && (strcmp(symb->id, id) == 0) ){
          CHERCHE_SYMB_GLOBAL = 0;
          return ctxt_suiv->symb; // symbole trouve dans le contexte
        }
        ctxt_suiv = ctxt_suiv->suiv;
      }
      break; // contexte trouve -> fin de boucle
    }
    suiv = suiv->suiv;
  }

  if (strcmp(ts->contexte, "GLOBAL") != 0)
    ErrorTs("contexte GLOBAL non existant");

  ctxt_suiv = ts->table_ctxt;// cherche symbole dans GLOBAL
  while (ctxt_suiv != NULL) { //Parcours de GLOBAL
    if( type == ctxt_suiv->symb->type && (strcmp(ctxt_suiv->symb->id, id) == 0) ){
      CHERCHE_SYMB_GLOBAL = 1;
      return ctxt_suiv->symb; // id trouve dans GLOBA
    }
    ctxt_suiv = ctxt_suiv->suiv;
  }

  return NULL;
}

symbole * RechercherADR(table_symb * ts, char * id){
  symbole* s;
  s = RechercherSymb(ts, id, TS_ADR);
  if (s != NULL)
    return s;
  s = RechercherSymb(ts, id, TS_IDL_DECLA);
  if (s != NULL)
    return s;
  return RechercherSymb(ts, id, TS_DECLA);
}

void setCONTEXTE(char* ctxt){
  strcpy(CONTEXTE,ctxt);
}

void printTS(table_symb * ts){
  if(ts == NULL)
    return;
  table_symb * suiv = ts;
  while (suiv != NULL) {
    printf(TXT_BOLD TXT_GREEN "CONTEXT:  " TXT_NULL "%s\n", suiv->contexte);
    printCONTEXTE(suiv->table_ctxt);
    suiv = suiv->suiv;
  }
}

void printCONTEXTE(contexte * tc){
  if(tc == NULL)
    return;
  contexte * ctxt_suiv = tc;
  symbole * s;
  while (ctxt_suiv != NULL) { //Parcours du contexte
    s = ctxt_suiv->symb;
    if(s == NULL)
      return;
    printf(TXT_BOLD "    ID:  " TXT_NULL "%s ", s->id);
    printf(TXT_BOLD "ADR:  " TXT_NULL "%d ", s->adr);
    printf(TXT_BOLD "TYPE:  " TXT_NULL "%s \n", STR_TYPE[ s->type - 64]);
    ctxt_suiv = ctxt_suiv->suiv;
  }
}


void ErrorTs(const char * errmsg){
  fprintf(stderr,"[TS error] %s\n",errmsg);
  exit(1);
}
