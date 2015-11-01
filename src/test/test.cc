#include "../MiniSQL.h"
#include "../btree.h"
#include "../global.h"
#include <iostream>
#include <string.h>
int main()
{
    table tb;
    column col;
    col.type = INT;
    strcpy_s(col.name_str,255, "Hello");
    col.size_u8 = 4;
    col.unique_u8 = true;
    strcpy_s(tb.name_str,255, "test");
    btree_create(&tb, "0", &col);
    item i;
    i.data.i = 1;
    i.type = INT;
    item i2;
    i2.data.i = 2;
    i2.type = INT;
    btree_insert("0", i, 0);
    btree_insert("0", i2, 1);
}