#include "MiniSQL.h"
#include "catalog.h"
#include <string.h>
bool miniSQL_createTable(table* tb)
{
    catalog_createTable(tb);
    char tmp[259];
    strcpy_s(tmp,255, tb->name_str);
    strcat(tmp,".db");
    buffer_init(&(tb->buf), tmp);
    return true;
}

table* miniSQL_connectTable(const char* tablename)
{
    table *tb = new table;
    char tmp[259];
    strcpy_s(tmp,255, tablename);
    strcat(tmp, ".db");
    buffer_init(&(tb->buf), tmp);
    catalog_connectTable(tb);
    return tb;
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