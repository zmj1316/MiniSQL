%{
#define YYSTYPE void*
#include <stdio.h>
#include <iostream>
#include <stdio.h>
using namespace std;
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
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
record rcd;
Filter filter;
typedef struct{
	char * name;
	Compare cmp;
	item i;
} TData;
vector<TData> Tdatas;
Compare cmp;
extern FILE *yyin;
%}
%token STRING 
%token CREATE DROP SELECT INSERT DELETE QUIT EXEC
%token TABLE FROM INTO mLT mLE mEQ mGT mGE mNE STAR
%token tCHAR tFLOAT tINT 
%token NAME INTEGER FF LS RS TM WHERE DD NEWLINE
%token VALUES CC UNI PRIM KEY INDEX ON AND FNAME
%%
program      : statements 
			 ;

statements   : statement
			 | statements  statement
			 ;

statement 	:  SELECT_S		{printf("select done\n");}
			|  CREATE_S		{printf("create done\n");}
			|  DROP_S		{printf("drop done\n");}
			|  DELETE_S		{printf("delete done\n");}
			|  INSERT_S     {printf("insert done\n");}
			|  QUIT			{exit(0);}
			|  EXEC NAME    {
							close(0);
							fprintf(stdout,"Opening %s\n",(char*)$2);
    						if ( freopen((char*)$2, "r", stdin) == NULL ){
    						    fprintf(stderr,"Could not open %s\n",(char*)$2);
    						    return 1;
    						}
					}
			;

SELECT_S 	 : SELECT  STAR FROM NAME WHERE TJS  TM {
				table *tb;
				if((tb = miniSQL_connectTable((char*)$4)) == NULL){
					fprintf(stderr,"table %s not exist.\n",(char*)$4);
				}
				Rule rule;
				for (vector<TData>::iterator i = Tdatas.begin();i!=Tdatas.end();++i){
					int i0;
					for(i0 = 0; i0 < tb->colNum_u64;++i0){
						if(0==strcmp((*i).name,tb->col[i0].name_str)){
							if((*i).i.type==tb->col[i0].type){
								rule.colNo = i0;
								rule.cmp = (*i).cmp;
								rule.target = (*i).i.data;
								break;
							}
							else if((*i).i.type == INT && tb->col[i0].type == FLOAT){
								rule.colNo = i0;
								rule.cmp = (*i).cmp;
								rule.target.f = (float)(*i).i.data.i;
								break;
							}
							else{
								fprintf(stderr,"type of %s not match!\n",(*i).name);
								goto SSE;
							}
						}
					}
					if(i0 == tb->colNum_u64){
						fprintf(stderr,"No column matching filter %s!\n",(*i).name);
						goto SSE;
					}
					filter.rules.push_back(rule);
				}
				printVrecord(miniSQL_select(tb,&filter));
				SSE:
				filter.rules.clear();
				Tdatas.clear();
}
			 | SELECT  STAR FROM NAME TM
			 {
			 	table *tb;
				if((tb = miniSQL_connectTable((char*)$4)) == NULL){
					fprintf(stderr,"table %s not exist.\n",(char*)$4);
					return 0;
				}
				filter.rules.clear();
				printVrecord(miniSQL_select(tb,&filter));
				Tdatas.clear();
			 }
			 ;

TJS			: TJ
			| TJS AND TJ
			;

TJ			: NAME FL INTEGER {
				TData t;
				t.name = (char*) $1;
				t.cmp = cmp;
				t.i.data.i = *(int*)$3;
				t.i.type = INT;
				Tdatas.push_back(t);
}
			| NAME FL FF	{
				TData t;
				t.name = (char*) $1;
				t.cmp = cmp;
				t.i.data.f = *(float*)$3;
				t.i.type = FLOAT;
				Tdatas.push_back(t);
				}
			| NAME FL STRING	{
				TData t;
				t.name = (char*) $1;
				t.cmp = cmp;
				t.i.data.str = new char[256];
				strcpy(t.i.data.str,(char*)$3);
				t.i.type = CHAR;
				Tdatas.push_back(t);
				}
			;



CREATE_S 	 : CREATE TABLE NAME CC ATTRS DD PRIM KEY CC NAME CC CC TM {
				strcpy(tb.name_str,(char*)$3);
				for(int i = 0; i < tb.colNum_u64;i++){
					if (strcmp((char*)$10,tb.col[i].name_str) == 0)
					{
						tb.primarykey_u8 = i;
						tb.col[i].unique_u8 = 1;
						if(!miniSQL_createTable(&tb)){
							fprintf(stderr,"create failed.\n");
							goto CCE;
						}
						table *tp = miniSQL_connectTable((char*)$3);
						if(tp == NULL) {fprintf(stderr,"Create failed!\n");return 0;}
						
						miniSQL_createIndex(tp,(char*)$10,(char*)$10);
						miniSQL_disconnectTable(tp);
						tb.colNum_u64=0;
					}
				}
				if(tb.colNum_u64 > 0)
				{
					fprintf(stderr, "Primary Key %s not Exist!\n", (char*)$10);
					tb.colNum_u64=0;
				}
				CCE:
				tb.name_str[0]=0;
				tb.colNum_u64=0;
				tb.recordSize = 0;
				}
			 | CREATE INDEX NAME ON NAME CC NAME CC TM
			 {
			 	table * tp = miniSQL_connectTable((const char*)$5);
				if(tp == NULL) {fprintf(stderr,"Table %s Not Exist!\n",(char*)$5); return 0;}
				//puts((char*)$3);
				miniSQL_createIndex(tp,(char*)$7,(char*)$3);
				miniSQL_disconnectTable(tp);
			 };

ATTRS		 : ATTR 	{}
			 | ATTRS  DD ATTR {}
			 ;

ATTR 		 : NAME tCHAR CC INTEGER CC {
				int i = tb.colNum_u64++;
				column *col = &tb.col[i];
				strcpy(col->name_str,(char*)$1);
				col->type = CHAR;
				col->unique_u8 = 0;
				col->size_u8 = *(long*)$4 + 1;
				tb.recordSize+=col->size_u8;
}
			 | NAME tCHAR CC INTEGER CC UNI{
			 	int i = tb.colNum_u64++;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = CHAR;
			 	col->unique_u8 = 1;
			 	col->size_u8 = *(long*)$4 + 1;
				tb.recordSize+=col->size_u8;

			 }
			 | NAME tINT{
			 	int i = tb.colNum_u64++;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = INT;
			 	col->unique_u8 = 0;
			 	col->size_u8 = 4;
				tb.recordSize+=col->size_u8;

			 }
			 | NAME tINT UNI{
			 	int i = tb.colNum_u64++;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = INT;
			 	col->unique_u8 = 1;
			 	col->size_u8 = 4;
				tb.recordSize+=col->size_u8;

			 }
			 | NAME tFLOAT{
			 	int i = tb.colNum_u64++;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = FLOAT;
			 	col->unique_u8 = 0;
			 	col->size_u8 = 4;
				tb.recordSize+=col->size_u8;

			 }
			 | NAME tFLOAT UNI{
			 	int i = tb.colNum_u64++;
			 	column *col = &tb.col[i];
			 	strcpy(col->name_str,(char*)$1);
			 	col->type = FLOAT;
			 	col->unique_u8 = 1;
			 	col->size_u8 = 4;
				tb.recordSize+=col->size_u8;

			 }
			 ;

INSERT_S     : INSERT INTO NAME VALUES CC VALUESS CC TM{
				table * tp = miniSQL_connectTable((const char*)$3);
				if(tp == NULL) {fprintf(stderr,"Table %s Not Exist!\n",(char*)$3); return 0;}
				if(rcd.i.size()!=tp->colNum_u64){
					fprintf(stderr,"column number not match, get %d |expect %d.\n",rcd.i.size(),tp->colNum_u64);
					goto IIE;
				}
				for(int i = 0 ; i < tp->colNum_u64; i++){
					if(tp->col[i].type!=rcd.i[i].type){
						if(rcd.i[i].type == INT && tp->col[i].type == FLOAT){
							rcd.i[i].type = FLOAT;
							rcd.i[i].data.f = (float)rcd.i[i].data.i;
						}
						else{
							fprintf(stderr,"Value not match type!");
							return 0;
						}
					}
				}
				rcd.valid = true;
				miniSQL_insert(tp, &rcd);
				IIE:
				miniSQL_disconnectTable(tp);
				rcd.i.clear();
}
			  ;

VALUE 		 : STRING	{
					item i;
					i.type = CHAR;
					i.data.str = new char[256];
					strcpy(i.data.str,(char*)$1);
					rcd.i.push_back(i);
					}
			 | INTEGER	{
			 		item i;
			 		i.type = INT;
			 		i.data.i = *(int*)$1;
			 		rcd.i.push_back(i);
			 }
			 | FF		{
			 		item i;
			 		i.type = FLOAT;
			 		i.data.f = *(float*)$1;
			 		rcd.i.push_back(i);
			 }
			 ;

VALUESS      : VALUE
			 | VALUESS DD VALUE
			 ;

FL 			 : mLE	{cmp = LE;}
			 | mLT	{cmp = LT;}
			 | mEQ	{cmp = EQ;}
			 | mNE	{cmp = NE;}
			 | mGE	{cmp = GE;}
			 | mGT	{cmp = GT;}
			 ;

DELETE_S	 : DELETE FROM NAME WHERE TJS TM{
				table *tb;
				if((tb = miniSQL_connectTable((char*)$4)) == NULL){
					fprintf(stderr,"table not exist.");
					return 0;
				}
				Rule rule;
				for (vector<TData>::iterator i = Tdatas.begin();i!=Tdatas.end();++i){
					for(int i0 = 0; i0 < tb->colNum_u64;++i0){
						if(0==strcmp((*i).name,tb->col[i0].name_str)){
							if((*i).i.type==tb->col[i0].type){
								rule.colNo = i0;
								rule.cmp = (*i).cmp;
								rule.target = (*i).i.data;
								break;
							}
							else if((*i).i.type == INT && tb->col[i0].type == FLOAT){
								rule.colNo = i0;
								rule.cmp = (*i).cmp;
								rule.target.f = (float)(*i).i.data.i;
								break;
							}
							else{
								fprintf(stderr,"type of %s not match!",(*i).name);
								return 0;
							}
						}
					}
					filter.rules.push_back(rule);
				}
				fprintf(stdout,"%d rows deleted.",miniSQL_delete(tb,&filter));
				filter.rules.clear();
				Tdatas.clear();		
				miniSQL_disconnectTable(tb);		
}
			 | DELETE FROM NAME TM{
			 	table *tb;
				if((tb = miniSQL_connectTable((char*)$4)) == NULL){
					fprintf(stderr,"table not exist.");
					return 0;
				}
				filter.rules.clear();
				fprintf(stdout,"%d rows deleted.",miniSQL_delete(tb,&filter));
				Tdatas.clear();
				miniSQL_disconnectTable(tb);
			 }


DROP_S   	 : DROP TABLE NAME TM{
				table *tb;
				puts("drop table");
				if((tb = miniSQL_connectTable((char*)$3)) == NULL){
					fprintf(stderr,"table %s not exist.\n",(char*)$3);
					return 0;
				}
				miniSQL_dropTable(tb);
}
			 | DROP INDEX NAME TM{
				miniSQL_dropIndex((char*)$3);
			 }
			 ;			 
%%
