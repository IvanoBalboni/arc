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
ID              [a-zA-Z_][a-zA-Z_0-9]*


%%

[ \t\n]         { /* ignorer les blancs */ }
{NOMBRE}        { yylval.nb = atoll(yytext); return NB; }
"MAIN()"        { return MAIN;}
"VAR"           { return VAR;}
"DEBUT"         { return DEBUT;}
"FIN"           { return FIN;}
"TQ"            { return TQ;}
"FAIRE"         { return FAIRE;}
"FTQ"           { return FTQ;}
";"             { return SEP;}
"<-"            { return AFFECT;}
[-+*/()]        { return *yytext;}
{ID}            { strcpy(yylval.id, yytext); return ID;}
.               {
                  sprintf(errmsg,charerr, yytext[0]);
		  yyerror(errmsg);
		  return 1;
                }

%%
