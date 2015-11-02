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
    //Rule r;
    //r.target.str = "Key123";
    //r.cmp = LE;
    //r.colNo = 0;
    //auto res=btree_select("0", &r);
    //for (size_t i = 0; i < res.size(); i++)
    //{
    //    cout << res[i] << endl;
    //}
    Data ii;
    char tmp[255];
    long long a = time(NULL);
    for (size_t i = 0; i < 20000; i++)
    {
        //ii.data.i = i;
        sprintf(tmp, "Key%d", i);
        ii.str = tmp;
        btree_insert("0", &ii, i);
    }
    printf("%lld", time(NULL) - a);
        //travel("0");
    system("pause");
}
