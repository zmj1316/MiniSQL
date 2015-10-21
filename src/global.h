/*
 * (C) 2015 Key Zhang
 * @global.h
 * @Created on 2015/10/20
 * @Defined some data types and internal functions
 */

#ifndef _GLOBAL_H
#define _GLOBAL_H
//#include "catalog.h"
#include "list.h"




typedef long i64;                  /* 8-byte signed integer */
typedef unsigned long u64;         /* 8-byte unsigned integer */
typedef unsigned int u32;          /* 4-byte unsigned integer */
typedef unsigned short u16;        /* 2-byte unsigned integer */
typedef short i16;                 /* 2-byte signed integer */
typedef unsigned char u8;          /* 1-byte unsigned integer */
typedef char i8;                   /* 1-byte signed integer */
typedef int Bool;                  /* Boolean: 0=True -1=False*/
#define  True 0
#define  False -1
void *mallocZero(u64);


/* Buffer Section */
/* Todo : transfer the implementation to buffer.h*/
struct Buffer
{
    int a;
};
typedef struct Buffer Buffer;
void *getBuffer(const char *);

/* BTree Section */
/* Todo : transfer the implementation to btree.h*/
struct BTree
{
    //Buffer *buf;
    int a;
};
typedef struct BTree BTree;
void *getBtree(Buffer *);



enum dataType
{
    INT, CHAR, FLOAT
};

struct miniSQL
{
    Buffer *buf;
    BTree *bTree;
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
    u64 colNum_u64;
    u8 primarykey_u8;
};
typedef struct table table;

struct record
{
    dataType type;
    union Data
    {
        int i;
        float f;
        char *str;
    }data;

};

enum Compare
{
    LT,LE,EQ,GE,GT
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


#endif                             /* _GLOBAL_H */

