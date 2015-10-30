#include "MiniSQL.h"
#include "catalog.h"
#include <stdlib.h>

bool miniSQL_createTable(table* tb)
{
    catalog_createTable(tb);
    strcpy(tb->buf.filename , tb->name_str);
    strcpy(tb->buf.filename, ".db");
    tb->buf.dirty = false;
    return true;
}
