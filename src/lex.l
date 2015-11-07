%option noyywrap
%option nounput
%option yylineno
%{ 
#define YYSTYPE void *
#include <iostream>     
#include <stdlib.h>
#include <limits.h>
#include "ra.tab.h"
using namespace std;

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
// const int32_t MAX_STR_CONST = 0x9000;
char string_buf[0x9000];
char *string_buf_ptr;
// extern int32_t yylineno;
%}
digit    [0-9]
hexdigit [0-9a-fA-F]
alpha    [a-zA-Z]
id       [a-zA-Z][a-zA-Z0-9_]*
ws       [ \t]+

%x string

%%
[ \t\r\n]*"#".*  /*ignore comment lines*/

{ws}  /* ignore whitespace */

[ \t\r\n]+ {
 // found a newline. should terminate current context, etc. 
 // collapses multiple blank lines to a single newline.
 // return NEWLINE;
}



"create" 	{return CREATE; }
"drop"		{return DROP;}
"select"	{return SELECT;}
"insert"	{return INSERT;}
"delete"	{return DELETE;}
"quit"		{return QUIT;}
"execfile"	{return EXEC;}	
"table"		{return TABLE;}
"from"		{return FROM;}
"into"		{return INTO;}
"where"		{return WHERE;}
"<" 		{return mLT;}
"<="		{return mLE;}
"="			{return mEQ;}
"<>"		{return mNE;}
">"			{return mGT;}
">="		{return mGE;}
"char"		{return tCHAR;}
"float"		{return tFLOAT;}
"int" 		{return tINT;}
";"			{return TM;}
"*"			{return STAR;}
","			{return DD;}
"values"	{return VALUES;}
"{"         {return LS;}
"}"         {return RS;}

"and"       {return AND;}
"unique"    {return UNI;}
"primary"   {return PRIM;}
"key"       {return KEY;}

"index"     {return INDEX;}
"on"        {return ON;}

{id} {
  yylval = _strdup(yytext);
  //puts((char*)yylval);
  return NAME;
}

[a-zA-Z0-9_]+\.*[a-zA-Z0-9_]* {
  yylval = _strdup(yytext);
  puts("Fname");
  return FNAME;
}

[()] {return CC;}

\'[^\']*\' {
  char *tmp =  _strdup(yytext);
  tmp[strlen(tmp) - 1]=0;
  yylval = tmp+1;
  //puts((char*)yylval);
  return STRING;
}

\-?{digit}+\.{digit}+ {
  // found an float
  float * f = (float*)malloc(sizeof(float));
  *f = strtof(yytext, NULL);
  yylval = f;
  return FF;
}

\-?{digit}+ {
  // found an integer
  long *f = (long*)malloc(sizeof(long));
  *f =  strtol(yytext, NULL, 10);
  yylval = f;
  return INTEGER;
}


. {
  cerr << "invalid character; line " << yylineno << _strdup(yytext) << endl;
}
%%
