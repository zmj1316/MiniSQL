/*
 * (C) 2015 Key Zhang
 * @global.c
 * @Created on 2015/10/20
 * @Defined some data types and internal functions
 */

#ifndef _GLOBAL_H
#define _GLOBAL_H

/* Buffer Section */
struct Buffer
{
    int a;
};
typedef struct Buffer Buffer;
void *getBuffer(const char *);

/* BTree Section*/
struct BTree
{
    int a;
};
typedef struct BTree BTree;



struct miniSQL
{
    /* data */
    Buffer *buf;
    BTree *bTree;
};
typedef miniSQL miniSQL;

typedef long i64;                  /* 8-byte signed integer */
typedef unsigned long u64;         /* 8-byte unsigned integer */
typedef unsigned int u32;          /* 4-byte unsigned integer */
typedef unsigned short u16;        /* 2-byte unsigned integer */
typedef short i16;                 /* 2-byte signed integer */
typedef unsigned char u8;          /* 1-byte unsigned integer */
typedef char i8;                   /* 1-byte signed integer */

void *mallocZero(u64);
#endif                             /* _GLOBAL_H */

