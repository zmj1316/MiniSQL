/*
 * (C) 2015 Key Zhang
 * @MiniSQL.h
 * @Created on 2015/10/19
 */
#ifndef _MINISQL_H
#define _MINISQL_H
#include "global.h"
#include "buffer.h"
#include <iostream>
#include <vector>
using namespace std;
/******************/
/* Data Structures*/
/******************/

union Data
{
    int i;
    float f;
    char *str;
};
typedef union Data Data;

class column
{
public:
    char name_str[255];// column name
    dataType type;// data type
    bool unique_u8;// unique?
    u8 size_u8;// size: int&float->4 str->0~255
};

class table
{
public:
    char name_str[255];// table name
    column col[MAXCOLUMN];// columns
    u64 colNum_u64;// column number
    u8 primarykey_u8;// Primary key
    Buffer buf;// the buffer 
    u32 recordSize;// The size of one piece of record
    u32 recordNum;// number of records
};

class item
{
public:
    dataType type;// datatype
    Data data;// item data
};

class record
{
public:
    vector<item> i;
    bool valid;// valid flag (??)
};

enum Compare
{
    LT, LE, EQ, GE, GT // Compare type
};
typedef enum Compare Compare;
class Rule
{
public:
    u32 colNo;// compare source index
    Compare cmp;// compare type
    Data target;// compare target
};

class Filter
{
public:
    //MiniList rules_list;
    vector<Rule> rules;
};

/*********************/
/* Public Functions  */
/*********************/
//Bool miniSQL_open(
//    const char *,       /* Database filename  */
//    miniSQL **          /* OUT: MiniSQL db handle */
//    );

Bool miniSQL_createTable(
    table *             /* Table */
    );

table *miniSQL_connectTable(
    const char*filename
    );

Bool miniSQL_dropTable(
    table *         /* Table name*/
    );

vector<record> miniSQL_select(
    table *,
    Filter *
    );
Bool miniSQL_insert(
    table *,
    const record *
    );
Bool miniSQL_delete(
    table *,
    Filter *
    );

void miniSQL_disconnectTable(
    table *
    );
/***************/

 #endif /* _MINISQL_H */
