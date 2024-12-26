%{
  #include <stdio.h>
  #include <ctype.h>
  #include <string.h>

  #include "ast.h"
  #include "ts.h"
  #include "codegen.h"
  #include "semantic.h"

  extern int yylex();
  static void print_file_error(char * s, char *errmsg);

  struct ast * ARBRE_PRE_MAIN = NULL;
  struct ast * ARBRE_ABSTRAIT = NULL;
  struct ast * ARBRE_DECLARATION = NULL;

  table_symb * TABLE_SYMBOLES = NULL;

  void yyerror(const char * s);

  char srcname[64];
  char exename[64] = "a.out";
  FILE * exefile;
%}

%union{
  int         nb;
  char        id[32];
  struct ast* tree;
 };

%define parse.error detailed
%locations

%type <tree> PROGRAMME PRE_MAIN FONCTION INSTRUCTIONS EXP DECLARATIONS
%type <tree> INST DECLA L_ID LISTE AFFECTATION L_PARAM

%token MAIN ALGO LIRE ECRIRE RETOURNE VAR DEBUT FIN SEP AFFECT ','
%token TQ FAIRE FTQ
%token SI SINON FSI
%token <nb> NB
%token <id> ID
%start PROGRAMME

%left  OU
%left  ET
%left  EGAL DIFF
%left  INF '<' SUP '>'
%left  '+' '-'
%left  '*' '/' '%'
%token '(' ')' '[' ']'

%%

PROGRAMME:
PRE_MAIN
MAIN
DECLARATIONS
DEBUT
INSTRUCTIONS
FIN                             {
                                  ARBRE_PRE_MAIN = $1;
                                  $$ = $5; ARBRE_ABSTRAIT = $$;
                                  ARBRE_DECLARATION = $3;
                                }
;

PRE_MAIN: %empty                { $$ = NULL;}
| VAR DECLA SEP PRE_MAIN        { $$ = CreerNoeudINSTRUCT($2, $4); }
| FONCTION PRE_MAIN             { $$ = CreerNoeudINSTRUCT($1, $2); }
;

FONCTION:
ALGO ID '(' L_PARAM ')'
DECLARATIONS
DEBUT
INSTRUCTIONS
FIN                             { $$ = CreerFONCTION(yyval.id, $4, $6, $8);}
;

L_PARAM: %empty                 { $$ = NULL; }
|L_ID                           { $$ = $1;   }
;

L_ID: ID                        { $$ = CreerNoeudLISTE( CreerFeuilleID($1) , NULL); }
| ID ',' L_ID                   { $$ = CreerNoeudLISTE( CreerFeuilleID($1) , $3);}
;

LISTE: EXP                      { $$ = CreerNoeudLISTE($1, NULL); }
| EXP ',' LISTE                 { $$ = CreerNoeudLISTE($1, $3);   }
;

DECLARATIONS: %empty            { $$ = NULL;}
| VAR DECLA SEP DECLARATIONS    { $$ = CreerNoeudINSTRUCT($2, $4);}
;


INSTRUCTIONS: INST SEP          { $$ = CreerNoeudINSTRUCT($1, NULL); }
| INST SEP INSTRUCTIONS         { $$ = CreerNoeudINSTRUCT($1, $3);   }
;

INST: EXP                       { $$ = $1; }
| ECRIRE EXP                    { $$ = CreerFeuilleECRIRE($2); }
| RETOURNE EXP                  { $$ = CreerFeuilleRETOURNE($2); }
| VAR DECLA                     { $$ = $2; }
| AFFECTATION                   { $$ = $1; }
| SI EXP FAIRE INSTRUCTIONS FSI { $$ = CreerNoeudSI($2, $4, NULL);}
| SI EXP FAIRE INSTRUCTIONS SINON INSTRUCTIONS FSI { $$ = CreerNoeudSI($2, $4, $6);}
| TQ EXP FAIRE INSTRUCTIONS FTQ {$$ = CreerNoeudTQ($2, $4);}
;

DECLA: ID AFFECT EXP            {$$ = CreerFeuilleDECLA(yyval.id, $3); /*TODO: marche pas*/ }
| ID '[' NB ']'                 {$$ = CreerFeuilleDECLALISTE(yyval.id, yyval.nb);}
| ID                            {$$ = CreerFeuilleDECLA(yyval.id, NULL);}
;

EXP : NB                        { $$ = CreerFeuilleNB(yyval.nb); }
| ID                            { $$ = CreerFeuilleID(yyval.id); }
| ID '[' EXP ']'                { $$ = CreerFeuilleLIRE_ELEM_LISTE(yyval.id, $3); }
| ID '(' LISTE ')'              { $$ = CreerAPPEL_FONCTION(yyval.id, $3); }
| EXP '+' EXP                   { $$ = CreerNoeudOP(OP_PLUS,  $1, $3); }
| EXP '-' EXP                   { $$ = CreerNoeudOP(OP_MOINS, $1, $3); }
| EXP '*' EXP                   { $$ = CreerNoeudOP(OP_MULT,  $1, $3); }
| EXP '/' EXP                   { $$ = CreerNoeudOP(OP_DIV,   $1, $3); }
| EXP '%' EXP                   { $$ = CreerNoeudOP(OP_MOD,   $1, $3); }
| EXP INF EXP                   { $$ = CreerNoeudOP(OP_INF,   $1, $3); }
| EXP '<' EXP                   { $$ = CreerNoeudOP(OP_SINF,  $1, $3); }
| EXP SUP EXP                   { $$ = CreerNoeudOP(OP_SUP,   $1, $3); }
| EXP '>' EXP                   { $$ = CreerNoeudOP(OP_SSUP,  $1, $3); }
| EXP EGAL EXP                  { $$ = CreerNoeudOP(OP_EGAL,  $1, $3); }
| EXP DIFF EXP                  { $$ = CreerNoeudOP(OP_DIFF,  $1, $3); }
| EXP OU EXP                    { $$ = CreerNoeudOP(OP_OU,    $1, $3); }
| EXP ET EXP                    { $$ = CreerNoeudOP(OP_ET,    $1, $3); }
| '(' EXP ')'                   { $$ = $2; }
| LIRE                          { $$ = CreerFeuilleLIRE(); }
;

AFFECTATION: ID AFFECT EXP     { $$ = CreerFeuilleAFFECT(yyval.id, $3); }
| ID '[' EXP ']' AFFECT EXP    { $$ = CreerFeuilleAFFECTLISTE(yyval.id, $3, $6);}
;

%%

int main( int argc, char * argv[] ) {
  //pour compiler example1 : ./bin/arc test/exemple1.algo a.out
  extern FILE *yyin;

  if (argc > 1){
    strcpy(srcname, argv[1]);
    if ( (yyin = fopen(srcname,"r"))==NULL ){
      char errmsg[256];
      sprintf(errmsg,"fichier \x1b[1m\x1b[33m' %s '\x1b[0m introuvable",srcname);
      print_file_error(argv[0],errmsg);
      exit(1);
    }
  }  else {
    print_file_error(argv[0],"aucun fichier en entree");
    exit(1);
  }
  if (argc == 3){
    strcpy(exename, argv[2]);
  }
  exefile = fopen(exename,"w+");
  yyparse();
  char indent[32] = "";
  printf(TXT_BOLD TXT_RED "arbre declarations\n");
  PrintAst( ARBRE_DECLARATION, indent);
  printf(TXT_BOLD TXT_RED "arbre instructions\n");
  PrintAst( ARBRE_ABSTRAIT, indent);

  TABLE_SYMBOLES = TableSymbInit();

  printf("hey\n" );

  setCONTEXTE("MAIN");
  printf("pb0\n");


  semantic(ARBRE_DECLARATION);
  printf("pb1\n");
  semantic(ARBRE_ABSTRAIT);
  printf("pb2\n");

  printTS(TABLE_SYMBOLES);
  printf("codegen oui\n" );

  codegenInit();
  printf("init pass\n");

  codegen(ARBRE_DECLARATION);
  printf("codegen 2\n" );
  codegenCHGT_PILE(0);
  codegen(ARBRE_ABSTRAIT);
  printf("codegen 3\n" );
  codegenFIN();
  fclose(yyin);
}



static void print_file_error(char * prog, char *errmsg){
  fprintf(stderr,
	  "\x1b[1m%s:\x1b[0m \x1b[31m\x1b[1merreur fatale:\x1b[0m %s\nechec de la compilation\n",
	  prog, errmsg);
}

void yyerror(const char * s)
{
  fprintf(stderr, "\x1b[1m%s:%d:%d:\x1b[0m %s\n", srcname, yylloc.first_line, yylloc.first_column, s);
  exit(0);
}
