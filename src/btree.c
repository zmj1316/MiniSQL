#include "btree.h"
#include "global.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <io.h>
bool btree_create(table* tb,const char * idxname, column *col)
{
    if (tb == NULL)
    {
        return false;
    }
    FILE *fp;
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename,".idx");
    if (access(filename,0)==0)
    {
        fprintf(stderr, "Index Exists Already!");
        return false;
    }
    strcpy_s(tb->buf.filename, 259, filename);
    newBlock(&tb->buf);
    move_window(&tb->buf,0);
    *((u32*)tb->buf.win) = 0;
    *((u32*)(tb->buf.win + PARENTBLOCK)) = ROOTMARK;
    *((u32*)(tb->buf.win + NEXTBLOCK)) = NONEXT;
    *((u32*)(tb->buf.win + NODESIZE)) = col->size_u8 + sizeof(u32);
    tb->buf.dirty = true;
}

bool btree_insert(table* tb, u32 index, item i)
{
    btree bt;
    getBtree(&bt, tb->name_str);
    node nd;
    getNode(&bt, &nd, 0);
    if (nd.N<bt.nodeSize)
    {
        for (size_t i = 0; i < nd.N; i++)
        {
            
        }
    }
}

void getBtree(btree* bt_ptr, char* idxname)
{
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename, ".idx");
    buffer_init(&bt_ptr->buf, filename);
    move_window(&(bt_ptr->buf), 0);
}

void getNode(btree* bt, node* nd, u32 block)
{
    u8 * bin = bt->buf.win;
    move_window(&bt->buf, block+1); 
    nd->parent = *((u32*)(bin + PARENTBLOCK));
    nd->N = *((u32*)bin + ENTRYCOUNT);
    nd->nodeNo = block;
    for (size_t i = 2; i < nd->N; i++)
    {
        u8* base = bin + bt->nodeSize * i;
        
        nd->childs[i] = *(u32*)(bin + sizeof(Data));
    }
}
