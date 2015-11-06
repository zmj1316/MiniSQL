#include "../MiniSQL.h"
#include "../btree.h"
#include "../global.h"
#include "../recordmanager.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
int test_btree_main()
{
    column col;
    col.type = CHAR;
    strcpy_s(col.name_str,255, "Hello");
    col.size_u8 = 200;
    col.unique_u8 = true;
    btree_create("0", &col);
    Data ii;
    char tmp[255];
    long long a = time(NULL);
    for (size_t i = 0; i < 10000; i++)
    {
        //ii.data.i = i;
        sprintf(tmp, "Key%d", i);
        ii.str = tmp;
        btree_insert("0", &ii, i);
    }
    printf("TIme:%llu\n", time(NULL) - a);
    Rule rr;
    rr.cmp = EQ;
    rr.colNo = 0;
    rr.target.str=new char[255];
    char *s = rr.target.str;
    strcpy(s, "Key1007");
    for (size_t i = 0; i < 1990; i++)
    {
        sprintf(s, "Key%u", i);
        btree_delete_node("0", &rr.target);
    }
    travel("0");
    
    //btree_delete("0", &rr);

    //btree_delete("0", &rr);

    //Rule r;
    //r.target.str = "Key101";
    //r.cmp = LT;
    //r.colNo = 0;
    //auto res=btree_select("0", &r);
    //for (size_t i = 0; i < res.size(); i++)
    //{
    //    cout << res[i] << endl;
    //}
    system("pause");
    return 0;
}

int main()
{
    table tb;
    strcpy(tb.name_str, "TestTb");
    tb.primarykey_u8 = 0;
    tb.recordSize = 4;
    tb.recordNum = 0;
    column * col = tb.col;;
    col->type = INT;
    col->idxname[0] = 0;
    strcpy(col->name_str, "col0");
    col->size_u8 = 4;
    col->unique_u8 = 0;
    tb.colNum_u64 = 1;
    miniSQL_createTable(&tb);
    table* tt = miniSQL_connectTable("TestTb");
    record rcd;
    item ii;
    ii.type = INT;
    ii.data.i = 100;
    rcd.i.push_back(ii);
    miniSQL_insert(tt, &rcd);
    miniSQL_disconnectTable(tt);
    return 0;
}
