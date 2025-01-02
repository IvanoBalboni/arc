%{
  #include <string.h>
  #include "parser.h"

  extern void yyerror(char *);

  char errmsg[256]="";
  const char charerr[] = "\x1b[1m\x1b[31m[erreur lexicale]\x1b[0m caractère \x1b[41m%c\x1b[0m inattendu";

  /* MACRO défini
   * Action executee avant chaque action semantique (meme vide)
   * et rempli la variable Bison `yylloc` avec la position du token
   */
#define YY_USER_ACTION                                             \
  yylloc.first_line = yylloc.last_line;                            \
  yylloc.first_column = yylloc.last_column;                        \
  if (yylloc.last_line == yylineno)                                \
    yylloc.last_column += yyleng;                                  \
  else {                                                           \
    yylloc.last_line = yylineno;                                   \
    yylloc.last_column = 1;					   \
  }




%}

%option nounput
%option noinput
%option yylineno

NOMBRE          [0-9]+
ID              [a-zA-Z_][-a-zA-Z_0-9]*


%%
"#".*           { /* ignorer les commentaires*/}
[ \t\n]         { /* ignorer les blancs */ }
{NOMBRE}        { yylval.nb = atoll(yytext); return NB; }
"MAIN()"        { return MAIN;}
"ALGO"          { return ALGO;}
"LIRE()"        { return LIRE;}
"ECRIRE"        { return ECRIRE;}
"AFFICHER"      { return ECRIRE;}
"RETOURNE"      { return RETOURNE;}
"RENVOYER"      { return RETOURNE;}
"VAR"           { return VAR;}
"DEBUT"         { return DEBUT;}
"FIN"           { return FIN;}
";"             { return SEP;}
"<-"            { return AFFECT;}
"SI"            { return SI;}
"SINON"         { return SINON;}
"FSI"           { return FSI;}
"TQ"            { return TQ;}
"FAIRE"         { return FAIRE;}
"ALORS"         { return FAIRE;}
"FTQ"           { return FTQ;}
"OU"            { return OU;}
"ET"            { return ET;}
"<="            { return INF;}
">="            { return SUP;}
"!="            { return DIFF;}
"="             { return EGAL;}
"["             { return '[';}
"]"             { return ']';}
[-+*/%()<>,@]   { return *yytext;}
{ID}            { strcpy(yylval.id, yytext); return ID;}
.               {
                  sprintf(errmsg,charerr, yytext[0]);
		  yyerror(errmsg);
		  return 1;
                }

%%
