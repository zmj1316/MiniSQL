#include "MiniSQL.h"
#include "catalog.h"
#include "recordmanager.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include "btree.h"

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
    fprintf(stdout, "droping table %s\n", tb->name_str);
    char filename[259];
    strcpy(filename, tb->name_str);
    strcat(filename, ".db");
    if (_access(filename, 0) != 0)
    {
        fprintf(stderr, "File %s not Exists!\n", filename);
        return false;
    }
    remove(filename);
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
    size_t i;
    for (i = 0; i < tb->colNum_u64; i++)
    {
        if (0==strcmp(tb->col[i].name_str,columnname))
        {
            strcpy(tb->col[i].idxname, indexname);
            break;
        }
    }
    if (i == tb->colNum_u64)
    {
        fprintf(stderr, "Column %s Not exists!", columnname);
        return false;
    }
    
    btree_create(tb->name_str, indexname, &tb->col[i]);
    Recordmanager_addindex(tb, indexname, i);
    return true;
}

bool miniSQL_dropIndex(const char* idxname)
{
    char tablename[256];
    char filename[259];
    strcpy(filename, idxname);
    strcat(filename, ".idx");
    strcpy(tablename,btree_getTable(idxname));
    table *tb = miniSQL_connectTable(tablename);
    for (size_t i = 0; i < tb->colNum_u64; i++)
    {
        if (0 == strcmp(idxname, tb->col[i].idxname))
        {
            tb->col[i].idxname[0] = 0;
            miniSQL_disconnectTable(tb);
            if (_access(filename, 0) == 0)// exist
            {
                remove(filename);
                return true;
            }
            else
            {
                fprintf(stderr, "Missing Index File!\n");
                return false;
            }
            
        }
    }
    fprintf(stderr, "Index %s not exist!\n",idxname);
    miniSQL_disconnectTable(tb);
    return false;
}

void miniSQL_disconnectTable(table* tb)
{
    catalog_disconnectTable(tb);
}

void printRecord(record r)
{
    //FILE *fp;
    //fp = fopen("Recordoutput.txt", "w");
    for (vector<item>::iterator it = r.i.begin(); it < r.i.end(); ++it)
    {
        item i = *it;
        switch (i.type)
        {
        default:
            break;
        case INT:
            fprintf(stdout, "INT: %d\n", i.data.i);
            break;
        case CHAR: 
            fprintf(stdout, "CHAR: %s\n", i.data.str);
            break;
        case FLOAT: 
            fprintf(stdout, "FLOAT: %f\n", i.data.f);
            break;
        }
    }
}

void printVrecord(vector<record> r)
{
    for (vector<record>::iterator it = (r).begin(); it < (r).end(); ++it)
    {
        printRecord((*it));
    }
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
