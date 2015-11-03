#include "../MiniSQL.h"
#include "../btree.h"
#include "../global.h"
#include <iostream>
#include <string.h>
#include <time.h>
int main()
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
    for (size_t i = 0; i < 200; i++)
    {
        //ii.data.i = i;
        sprintf(tmp, "Key%d", i);
        ii.str = tmp;
        btree_insert("0", &ii, i);
    }
    travel("0");
    Rule rr;
    rr.cmp = LT;
    rr.colNo = 0;
    rr.target.str="Key99";
    for (size_t i = 0; i < 99; i++)
    {
        printf("%u\n",i);
        btree_delete("0", &rr);
        //travel("0");
    }
    
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
    //system("pause");
}
