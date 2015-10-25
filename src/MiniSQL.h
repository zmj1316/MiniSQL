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
    char name_str[255];
    dataType type;
    u8 unique_u8;
    u8 size_u8;
};
typedef struct column column;


struct table
{
    char name_str[255];
    column *col;
    //MiniList *records;
    u64 colNum_u64;
    u8 primarykey_u8;
    Buffer *buf;
    u32 recordSize;
    u32 recordNum;
};
typedef struct table table;

struct item
{
    dataType type;
    Data data;
};
typedef struct item item;

struct record
{
    item i[MAXCOLUMN];
    Bool valid;
};
typedef struct record record;

enum Compare
{
    LT, LE, EQ, GE, GT
};
typedef enum Compare Compare;
struct Rule
{
    u32 colNo;
    Compare cmp;
    Data target;
};
typedef struct Rule Rule;

struct Filter
{
    MiniList rules_list;
};
typedef struct Filter Filter;
void Filter_add(Filter*, Rule *);



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

Bool miniSQL_connectTable(
    miniSQL *,
    table *
    );

Bool miniSQL_dropTable(
    miniSQL *,          /* DAtabase handle*/
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
/***************/

 #endif /* _MINISQL_H */
