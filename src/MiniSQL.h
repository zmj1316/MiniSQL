/*
 * (C) 2015 Key Zhang
 * @MiniSQL.h
 * @Created on 2015/10/19
 */
#ifndef _MINISQL_H
#define _MINISQL_H
#include "global.h"
#include "list.h"
#include "buffer.h"

/******************/
/* Data Structures*/
/******************/
struct miniSQL
{
    int a;
};
typedef struct miniSQL miniSQL;

typedef enum dataType dataType;

union Data
{
    int i;
    float f;
    char *str;
};

typedef union Data Data;
struct column
{
    char name_str[255];// column name
    dataType type;// data type
    Bool unique_u8;// unique?
    u8 size_u8;// size: int&float->4 str->0~255
};
typedef struct column column;


struct table
{
    char name_str[255];// table name
    column col[MAXCOLUMN];// columns
    u64 colNum_u64;// column number
    u8 primarykey_u8;// Primary key
    Buffer buf;// the buffer 
    u32 recordSize;// The size of one piece of record
    u32 recordNum;// number of records
};  
typedef struct table table;

struct item
{
    dataType type;// datatype
    Data data;// item data
};
typedef struct item item;

struct record
{
    item i[MAXCOLUMN];// 
    Bool valid;// valid flag (??)
};
typedef struct record record;

enum Compare
{
    LT, LE, EQ, GE, GT // Compare type
};
typedef enum Compare Compare;
struct Rule
{
    u32 colNo;// compare source index
    Compare cmp;// compare type
    Data target;// compare target
};
typedef struct Rule Rule;

struct Filter
{
    //MiniList rules_list;
    Rule rules[MAXCOLUMN];
};
typedef struct Filter Filter;
//void Filter_add(Filter*, Rule *);

/*********************/
/* Public Functions  */
/*********************/
Bool miniSQL_open(
    const char *,       /* Database filename  */
    miniSQL **          /* OUT: MiniSQL db handle */
    );

Bool miniSQL_createTable(
    miniSQL *,          /* Database handle */
    table *             /* Table */
    );

table *miniSQL_connectTable(
    const char*filename
    );

Bool miniSQL_dropTable(
    miniSQL *,          /* Database handle*/
    table *         /* Table name*/
    );

MiniList* miniSQL_select(
    miniSQL *,
    table *,
    Filter *
    );
Bool miniSQL_insert(
    miniSQL *,
    table *,
    MiniList *
    );
Bool miniSQL_delete(
    miniSQL *,
    table *,
    MiniList *
    );

void miniSQL_disconnectTable(
    table *
    );
/***************/

 #endif /* _MINISQL_H */
