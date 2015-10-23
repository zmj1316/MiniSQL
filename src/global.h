/*
 * (C) 2015 Key Zhang
 * @global.h
 * @Created on 2015/10/20
 * @Defined some data types and internal functions
 */

#ifndef _GLOBAL_H
#define _GLOBAL_H

#define BLOCKSIZE 4096


typedef long i64;                  /* 8-byte signed integer */
typedef unsigned long u64;         /* 8-byte unsigned integer */
typedef unsigned int u32;          /* 4-byte unsigned integer */
typedef unsigned short u16;        /* 2-byte unsigned integer */
typedef short i16;                 /* 2-byte signed integer */
typedef unsigned char u8;          /* 1-byte unsigned integer */
typedef char i8;                   /* 1-byte signed integer */
typedef int Bool;                  /* Boolean: 1=True 0=False*/

const int True = 1;
const int Flase = 0;

void *mallocZero(u64);


enum dataType
{
    INT, CHAR, FLOAT
};


#endif                             /* _GLOBAL_H */

