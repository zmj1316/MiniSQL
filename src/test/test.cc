#include "../MiniSQL.h"
#include "../btree.h"
#include <iostream>
#include <string.h>
int main()
{
    table tb;
    strcpy(tb.name_str, "test");
    btree_create(&tb, "0", 0);
    btree_create(&tb, "0", 0);

}