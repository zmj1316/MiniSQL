#include "../MiniSQL.h"
#include "../btree.h"
#include "../global.h"
#include <iostream>
#include <string.h>
int main()
{
    table tb;
    column col;
    col.type = CHAR;
    strcpy_s(col.name_str,255, "Hello");
    col.size_u8 = 5;
    col.unique_u8 = true;
    strcpy_s(tb.name_str,255, "test");
    btree_create(&tb, "0", &col);
    item i;
    i.data.str = "ABCD";
    i.type = CHAR;
    btree_insert("0", i, 0);
}