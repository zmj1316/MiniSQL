#ifndef _CATALOG_H
#define _CATALOG_H
#include "global.h"

//struct cmitem
//{
//    char *name_str;
//    dataType type;
//    u8 unique_u8;
//    u8 size_u8;
//};
//typedef struct cmitem cmitem;
//
//struct cmtable
//{
//    char *name_str;
//    cmitem *col_item;
//    u64 colNum_u64;
//    u8 primarykey_u8;
//};
//typedef struct cmtable cmtable;

Bool catalog_createTable(
    table *
    );
Bool catalog_connectTable(
    table *
    );
Bool catalog_dropTable(
    table *
    );

#endif
