/*
 * (C) 2015 Key Zhang
 * @MiniSQL.h
 * @Created on 2015/10/19
 */
#ifndef _MINISQL_H
#define _MINISQL_H
#include "global.h"
#include "buffer.h"
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
    char idxname[255]; // index name empty for No index
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

//enum Compare
//{
//    LT, LE, EQ, NE,GE, GT // Compare type
//};
//typedef enum Compare Compare;
typedef u8 Compare;
const u8 LT = 0;
const u8 LE = 1;
const u8 EQ = 2;
const u8 NE = 3;
const u8 GE = 4;
const u8 GT = 5;

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

bool miniSQL_createTable(
    table *             /* Table */
    );

table *miniSQL_connectTable(
    const char *tablename
    );

bool miniSQL_dropTable(
    table *         /* Table name*/
    );

vector<record> miniSQL_select(
    table *,
    Filter *
    );

bool miniSQL_insert(
    table *,
    record *
    );
u32 miniSQL_delete(
    table *,
    Filter *
    );
bool miniSQL_createIndex(
    table *,
    const char * columnname,
    const char * indexname
    );
bool miniSQL_dropIndex(
    //table *,
    //u32 index
    const char *
    );
void miniSQL_disconnectTable(
    table *
    );
/***************/

u8 Rule_cmp(dataType type, Data* s, Data *dst, Rule *);

 #endif /* _MINISQL_H */
