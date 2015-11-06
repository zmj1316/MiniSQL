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
void addColumn(table *tb, const char * colname, dataType type, const char * idxname,
    u8 size,u8 unique)
{
    column * col = &tb->col[tb->colNum_u64];
    strcpy(col->name_str, colname);
    col->type = type;
    strcpy(col->idxname, idxname);
    col->size_u8 = size;
    col->unique_u8 = unique==1;
    tb->colNum_u64++;
    tb->recordSize += size;
}
int main()
{
    table tb;
    memset(&tb, 0, sizeof(table));
    tb.recordSize = 0;
    tb.recordNum = 0;
    strcpy(tb.name_str, "student");
    addColumn(&tb, "sno", CHAR, "", 9, 1);
    addColumn(&tb, "sname", CHAR,"", 17, 1);
    addColumn(&tb, "sage", INT, "", 4, 0);
    addColumn(&tb, "sgender", CHAR, "", 2, 0);
    addColumn(&tb, "score", FLOAT, "", 4, 0);
    miniSQL_createTable(&tb);
    table* tt = miniSQL_connectTable("student");
    record r;
    item i;
    i.type = CHAR;
    i.data.str = "12345678";
    r.i.push_back(i);
    i.data.str = "wy1";
    r.i.push_back(i);
    i.type = INT;
    i.data.i = 22;
    r.i.push_back(i);
    i.type = CHAR;
    i.data.str = "M";
    r.i.push_back(i);
    i.type = FLOAT;
    i.data.f = 95;
    r.i.push_back(i);
    miniSQL_insert(tt, &r);
    r.i[0].data.str = "12345679";
    miniSQL_insert(tt, &r);
    miniSQL_disconnectTable(tt);
    tt = miniSQL_connectTable("student");
    miniSQL_select(tt, new Filter);
    return 0;
}
