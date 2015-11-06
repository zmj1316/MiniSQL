#include "MiniSQL.h"
#include "catalog.h"
#include "recordmanager.h"
#include <string.h>
#include <stdio.h>
#include <io.h>

bool miniSQL_createTable(table* tb)
{
    char filename[259];
    strcpy(filename, tb->name_str);
    strcat(filename, ".cat");
    if (_access(filename, 0) == 0)// exist
    {
        return false;
    }
    catalog_createTable(tb);
    char tmp[259];
    strcpy(tmp, tb->name_str);
    strcat(tmp,".db");
    buffer_init(&(tb->buf), tmp);
    return true;
}

table* miniSQL_connectTable(const char* tablename)
{
    table *tb = new table;
    char filename[259];
    strcpy(filename, tablename);
    strcat(filename, ".cat");
    if (_access(filename, 0) != 0)
    {
        return NULL;
    }
    char tmp[259];
    strcpy(tmp,tablename);
    strcat(tmp, ".db");
    buffer_init(&(tb->buf), tmp);
    strcpy(tb->name_str, tablename);
    catalog_connectTable(tb);
    return tb;
}

bool miniSQL_dropTable(table* tb)
{
    fprintf(stdout, "drop tb %s\n", tb->name_str);
    return catalog_dropTable(tb);
}

vector<record> miniSQL_select(table* tb, Filter* filter)
{
    return Recordmanager_selectRecord(tb, filter);
}

bool miniSQL_insert(table* tb, record* re)
{
    return Recordmanager_insertRecord(tb, re);
}

u32 miniSQL_delete(table* tb, Filter* f)
{
    return Recordmanager_deleteRecord(tb, f);
}

bool miniSQL_createIndex(table* tb, const char* columnname, const char* indexname)
{
    return true;
}

bool miniSQL_dropIndex(const char* idxname)
{
    return true;
}

void miniSQL_disconnectTable(table* tb)
{
    catalog_disconnectTable(tb);
    return;
}

u8 Rule_cmp(dataType type, Data* s, Data* dst, Rule* rule)
{
    float cmp = 0;
    switch (type)
    {
    case INT:
        cmp = s->i - dst->i;
        break;
    case CHAR:
        cmp = strcmp(s->str, dst->str);
        break;
    case FLOAT:
        cmp = s->f - dst->f;
        break;
    default:
        break;
    }
    switch (rule->cmp)
    {
    default:
        return -1;
    case LT:
        return cmp < 0 ? 1 : 0;
    case LE: 
        return cmp <= 0 ? 1 : 0;
    case EQ:
        return cmp == 0 ? 1 : 0;
    case NE: 
        return cmp != 0 ? 1 : 0;
    case GE: 
        return cmp >= 0 ? 1 : 0;
    case GT: 
        return cmp > 0 ? 1 : 0;
    }
}
