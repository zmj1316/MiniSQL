#include "btree.h"
#include "recordmanager.h"
bool btree_create(table* tb, u32 index)
{
    if (tb == nullptr)
    {
        return false;
    }
    FILE *fp;
    char filename[259];
    strcpy_s(filename, 255, tb->name_str);
    strcat_s(filename, 259, ".idx");
    fopen_s(&fp, filename, "wb");
    

}