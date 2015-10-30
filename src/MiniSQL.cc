#include "MiniSQL.h"
#include "catalog.h"
#include <string.h>
bool miniSQL_createTable(table* tb)
{
    //catalog_createTable(tb);
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
    //catalog_connectTable(tb);
    return tb;
}
