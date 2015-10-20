/*
 * (C) 2015 Key Zhang
 * @MiniSQL.c
 * @Created on 2015/10/19
 */
#ifndef _MINISQL_H
#define _MINISQL_H
#include "global.h"




int miniSQL_open(
  const char *,       /* Database filename  */
  miniSQL **          /* OUT: MiniSQL db handle */
);

int miniSQL_createTable(
    miniSQL *,      /* Database handle*/
    const char *    /* Table name*/

    );






 #endif /* _MINISQL_H */