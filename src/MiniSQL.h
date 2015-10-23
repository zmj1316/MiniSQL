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
    //List *records;
    u64 colNum_u64;
    u8 primarykey_u8;
    Buffer *buf;
    u64 recordSize;
};
typedef struct table table;

struct item
{
    dataType type;
    union Data
    {
        int i;
        float f;
        char *str;
    }data;
};
typedef struct item item;

enum Compare
{
    LT, LE, EQ, GE, GT
};
typedef enum Compare Compare;
struct Rule
{
    u32 colNo;
    Compare cmp;
    union Target
    {
        int i;
        float f;
        char *str;
    }target;
};
typedef struct Rule Rule;

struct Filter
{
    List rules_list;

};
typedef struct Filter Filter;
void Filter_add(Filter*, Rule *);


/*********************/
/* Public Functions  */
/*********************/
int miniSQL_open(
    const char *,       /* Database filename  */
    miniSQL **          /* OUT: MiniSQL db handle */
    );

int miniSQL_createTable(
    miniSQL *,          /* Database handle */
    table *             /* Table */
    );

int miniSQL_dropTable(
    miniSQL *,          /* DAtabase handle*/
    const char*         /* Table name*/
    );

List* miniSQL_select(
    miniSQL *,
    const char *,
    Filter *
    );

/***************/

 #endif /* _MINISQL_H */
