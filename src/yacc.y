%{
#define YYSTYPE void*
#include <iostream>
#include <stdio.h>
using namespace std;
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include "MiniSQL.h"
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
void yyerror(const char* s){
	cout << s;
}
extern int32_t yylineno;
int32_t yylex();
table tb;
%}
%token STRING 
%token CREATE DROP SELECT INSERT DELETE QUIT EXEC
%token TABLE FROM INTO mLT mLE mEQ mGT mGE mNE STAR
%token tCHAR tFLOAT tINT 
%token NAME INTEGER FF LS RS TM WHERE DD NEWLINE
%token VALUES CC UNI PRIM KEY INDEX ON AND
%%
program      : statements 
			 ;

statements   : statement
			 | statements  statement
			 ;

statement 	:  SELECT_S		{printf("select\n");}
			|  CREATE_S		{printf("create\n");}
			|  DROP_S		{printf("drop\n");}
			|  DELETE_S		{printf("delete\n");}
			|  INSERT_S     {printf("insert\n");}
			|  QUIT			{exit(0);}
			;

SELECT_S 	 : SELECT  STAR FROM NAME WHERE TJ  TM {}
			 | SELECT  STAR FROM NAME TM
			 ;

TJ			: NAME FL TAR
			| TJ AND NAME FL TAR
			;

TAR			: INTEGER {puts("TAR");}
			| FF	{puts("TAR");}
			| STRING	{puts((char*)$1);}
			;



CREATE_S 	 : CREATE TABLE NAME CC ATTRS DD PRIM KEY CC NAME CC CC TM {
				strcpy(tb.name_str,(char*)$3);
				for(int i = 0; i < tb.colNum_u64;i++){
					if (strcmp((char*)$10,tb.col[i].name_str) == 0)
					{
						tb.primarykey_u8 = i;
						tb.col[i].unique_u8 = 1;
						miniSQL_createTable(&tb);
						tb.colNum_u64=0;
					}
				}
				fprintf(stderr, "Primary Key not Exist!\n");
				exit(0);
				}
			 | CREATE INDEX NAME ON NAME CC NAME CC TM;

ATTRS		 : ATTR 	{}
			 | ATTRS  DD ATTR {}
			 ;

ATTR 		 : NAME tCHAR CC INTEGER CC {
				int i = ++tb.colNum_u64;
				column *col = &tb.col[i];
				strcpy(col->name_str,(char*)$1);
				col->type = CHAR;
				col->unique_u8 = 0;
				col->size_u8 = *(long*)$4;
}
			 | NAME tCHAR CC INTEGER CC UNI{
			 	int i = ++tb.colNum_u64;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = CHAR;
			 	col->unique_u8 = 1;
			 	col->size_u8 = *(long*)$4;
			 }
			 | NAME tINT{
			 	int i = ++tb.colNum_u64;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = INT;
			 	col->unique_u8 = 0;
			 	col->size_u8 = 4;
			 }
			 | NAME tINT UNI{
			 	int i = ++tb.colNum_u64;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = INT;
			 	col->unique_u8 = 1;
			 	col->size_u8 = 4;
			 }
			 | NAME tFLOAT{
			 	int i = ++tb.colNum_u64;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = FLOAT;
			 	col->unique_u8 = 0;
			 	col->size_u8 = 4;
			 }
			 | NAME tFLOAT UNI{
			 	int i = ++tb.colNum_u64;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = FLOAT;
			 	col->unique_u8 = 1;
			 	col->size_u8 = 4;
			 }
			 ;

INSERT_S     : INSERT INTO NAME VALUES CC VALUESS CC TM
			  ;

VALUE 		 : STRING	{puts((char*)$1);}
			 | INTEGER	{printf("%d\n",*(int*)$1);}
			 | FF		{printf("%f\n",*(float*)$1);}
			 ;

VALUESS      : VALUE
			 | VALUESS DD VALUE
			 ;

FL 			 : mLE	{puts("FL");}
			 | mLT	{puts("FL");}
			 | mEQ	{puts("FL");}
			 | mNE	{puts("FL");}
			 | mGE	{puts("FL");}
			 | mGT	{puts("FL");}
			 ;

DELETE_S	 : DELETE FROM NAME WHERE NAME FL TAR TM
			 | DELETE FROM NAME TM

DROP_S   	 : DROP TABLE NAME TM{}
			 | DROP INDEX NAME TM
			 ;			 
%%
