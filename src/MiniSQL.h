/*
 * (C) 2015 Key Zhang
 * @MiniSQL.h
 * @Created on 2015/10/19
 */
#ifndef _MINISQL_H
#define _MINISQL_H
#include "global.h"

#include "list.h"





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
    Filter *
    );


 #endif /* _MINISQL_H */
