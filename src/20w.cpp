/* This file is for debug only
 *
 *
 */
#include "MiniSQL.h"
int main()
{
    table *tb = miniSQL_connectTable("orders");
    record r;
    item i;
    i.type = INT;
    r.i.push_back(i);
    i.type = INT;
    r.i.push_back(i);
    i.type = CHAR;
    i.data.str = new char[2];
    r.i.push_back(i);
    i.type = FLOAT;
    r.i.push_back(i);
    i.type = CHAR;
    i.data.str = new char[16];
    r.i.push_back(i);
    i.type = CHAR;
    i.data.str = new char[80];
    r.i.push_back(i);
    char tmp[255];
    for (size_t i = 0; i < 20000; i++)
    {
        r.i[0].data.i = i;
        r.i[1].data.i = i * 23;
        strcpy(r.i[2].data.str, (i >> 3) % 2 == 1 ? "F" : "M");
        r.i[3].data.f = (float)i / 23;
        sprintf(tmp, "Clerk#%09d", i*3);
        strcpy(r.i[4].data.str, tmp);
        sprintf(tmp, "test check u%d", i);
        strcpy(r.i[5].data.str, tmp);
        miniSQL_insert(tb, &r);
        if(i%1000 == 0 )printf("%d\n", i);
    }
    miniSQL_disconnectTable(tb);
    return 0;
}

int main2()
{
    table *tb = miniSQL_connectTable("teacher");
    //tb->recordNum = 200000;
    Filter f;
    Rule r;
    r.target.str = "003";
    r.cmp = EQ;
    r.colNo = 0;
    f.rules.push_back(r);
    //r.target.f = 18;
    //r.cmp = LT;
    //r.colNo = 3;
    //auto rr = miniSQL_delete(tb, &f);
    //printf("%d\n", rr);
    miniSQL_delete(tb, &f);
    miniSQL_disconnectTable(tb);
    return 0;
}