#include "btree.h"
#include "global.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <io.h>
bool btree_create(table* tb,const char * idxname, column *col)
{
    if (tb == NULL)
    {
        return false;
    }
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename,".idx");
    if (_access(filename,0)==0)
    {
        fprintf(stderr, "Index Exists Already!");
        return false;
    }
    Buffer buf;
    strcpy_s(buf.filename, 259, filename);
    newBlock(&buf);
    move_window(&buf,0);
    *((u32*)buf.win) = 0;
    *((u32*)(buf.win + PARENTBLOCK)) = ROOTMARK;
    *((u32*)(buf.win + NEXTBLOCK)) = NONEXT;
    *((u32*)(buf.win + NODESIZE)) = col->size_u8 + sizeof(u32);
    *(u8*)(buf.win + DATATYPE) = col->type;
    buf.dirty = true;
    sync_window(&buf);
    return true;
}

bool btree_insert(table* tb, u32 index, item target)
{
    btree bt;
    getBtree(&bt, tb->name_str);
    node nd;
    getNode(&bt, &nd, 0);
    if (nd.N==0)
    {
        nd.N++;
        nd.datas[0] = target.data;
    }
    if (nd.N<bt.nodeSize)
    {
        for (size_t i = 0; i < nd.N; i++)
        {
            u8 cmp;
            switch (bt.type)
            {
            case INT: 
                cmp = nd.datas[i].i > target.data.i ? 1 : 0;
                break;
            case CHAR: 
                cmp = strcmp(nd.datas[i].str, target.data.str) ? 1 : 0;
                break;
            case FLOAT: 
                cmp = nd.datas[i].f > target.data.f ? 1 : 0;
                break;
            default:
                break;
            }
        }

    }
    return true;
}

void getBtree(btree* bt_ptr, char* idxname)
{
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename, ".idx");
    buffer_init(&bt_ptr->buf, filename);
    move_window(&(bt_ptr->buf), 0);
    bt_ptr->buf.dirty = true;
    bt_ptr->nodeSize = *(u32 *)(bt_ptr->buf.win + NODESIZE);
    bt_ptr->type = (dataType)*(bt_ptr->buf.win + DATATYPE);
    bt_ptr->capacity = (BLOCKSIZE - DATAHEAD - 1) / bt_ptr->nodeSize;
}

void getNode(btree* bt, node* nd, u32 block)
{
    u8 * bin = bt->buf.win;
    move_window(&bt->buf, block); 
    nd->parent = *((u32*)(bin + PARENTBLOCK));
    nd->N = *((u32*)bin + ENTRYCOUNT);
    nd->nodeNo = block;
    nd->childs = new u32[bt->capacity];
    nd->datas = new Data[bt->capacity];
    for (size_t i = 0; i < nd->N; i++)
    {
        u8* base = bin + bt->nodeSize * i + DATAHEAD;
        switch (bt->type)
        {
        case INT: 
            nd->datas[i].i = *(int*)base;
            break;
        case CHAR: 
            nd->datas[i].str = new char[bt->nodeSize - sizeof(u32)];
            memcpy(nd->datas[i].str, (char*)base, bt->nodeSize - sizeof(u32));
            break;
        case FLOAT: 
            nd->datas[i].f = *(float*)base;
            break;
        default: break;
        }
        base += bt->nodeSize - sizeof(u32);
        nd->childs[i] = *(u32*)(base + sizeof(Data));
    }
}
