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
    col.size_u8 = 200;
    col.unique_u8 = true;
    strcpy_s(tb.name_str,255, "test");
    btree_create(&tb, "0", &col);
    item ii;
    ii.type = col.type;
    char tmp[255];
    for (size_t i = 0; i < 1000; i++)
    {
        //ii.data.i = i;
        sprintf(tmp, "Key%d", i);
        ii.data.str = tmp;
        btree_insert("0", ii, i);
    }
    travel("0");
    system("pause");
}
