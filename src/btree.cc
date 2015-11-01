#include "btree.h"
#include "global.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <io.h>
/* create index of {col} in {tb} named {idxname}*/
bool btree_create(table* tb,const char * idxname, column *col)
{
    if (tb == NULL)
    {
        fprintf(stderr, "Table not Exists!");
        return false;
    }
    /* generate the index filename */
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename,".idx");
    /* check the index file */
    if (_access(filename,0)==0)
    {
        fprintf(stderr, "Index Exists Already!");
        return false;
    }
    /* buffer for write */
    Buffer buf;
    strcpy_s(buf.filename, 259, filename);
    newBlock(&buf);
    move_window(&buf,0);
    *((u32*)(buf.win + NODESIZE)) = col->size_u8 + sizeof(u32);
    *(u8*)(buf.win + DATATYPE) = col->type;
    *(u32*)(buf.win + BLOCKCOUNT) = 0;
    *(u32*)(buf.win + ROOTPTR) = 1;
    buf.dirty = true;
    sync_window(&buf);
    btree bt;
    getBtree(&bt, idxname);
    node n;
    newNode(&bt, &n);
    n.parent = ROOTMARK;
    n.next = NONEXT;
    freeNode(&bt, &n);
    freeBtree(&bt);
    return true;
}

bool btree_insert(const char *idxname, item target,u32 value)
{
    btree bt;
    if (!getBtree(&bt, idxname)) return false;
    node nd;
    findNode(&bt, &nd, &target.data);
    size_t i;
    for (i = 0; i < nd.N && cmp(bt.type, nd.datas[i], target.data)<=0; i++);
    insertData(&bt,&nd, i, &target.data, value);
    if (nd.N==bt.capacity) // split
    {
        node nnode;
        newNode(&bt, &nnode);

    }
    saveNode(&bt, &nd);
    return true;
}

bool getBtree(btree* bt_ptr, const char* idxname)
{
    char filename[259];
    strcpy_s(filename, 255, idxname);
    strcat(filename, ".idx");
    if (_access(filename, 0) != 0)
    {
        fprintf(stderr, "Index Don't Exists!");
        return false;
    }
    buffer_init(&bt_ptr->buf, filename);
    move_window(&(bt_ptr->buf), 0);
    bt_ptr->nodeSize = *(u32 *)(bt_ptr->buf.win + NODESIZE);
    bt_ptr->type = (dataType)*(bt_ptr->buf.win + DATATYPE);
    bt_ptr->capacity = (BLOCKSIZE - DATAHEAD) / bt_ptr->nodeSize - 1;
    bt_ptr->root = *(u32*)(bt_ptr->buf.win + ROOTPTR);
    bt_ptr->blockcount = *(u32*)(bt_ptr->buf.win + BLOCKCOUNT);
    return true;
}

void freeBtree(btree* bt)
{
    move_window(&bt->buf, 0);
    bt->buf.dirty = true;
    *(u32*)(bt->buf.win + BLOCKCOUNT) = bt->blockcount;
    sync_window(&bt->buf);
}

void getNode(btree* bt, node* nd, u32 block)
{
    u8 * bin = bt->buf.win;
    move_window(&bt->buf, block); 
    nd->parent = *((u32*)(bin + PARENTBLOCK));
    nd->N = *((u32*)bin + ENTRYCOUNT);
    nd->nodeNo = block;
    nd->next = *(u32*)(bin + NEXTBLOCK);
    nd->childs = new u32[bt->capacity + 1];
    nd->datas = new Data[bt->capacity + 1];
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
        nd->childs[i] = *(u32*)(base);
    }
}

void newNode(btree* bt, node* nd)
{
    bt->blockcount++;
    move_window(&bt->buf, bt->blockcount);
    nd->N = 0;
    nd->nodeNo = bt->blockcount;
    nd->next = 0;
    nd->childs = new u32[bt->capacity + 1];
    nd->datas = new Data[bt->capacity + 1];
}

void freeNode(btree* bt, node* nd)
{
    saveNode(bt, nd);
    if (bt->type==CHAR)
    {
        for (size_t i = 0; i < nd->N; i++)
        {
            free(nd->datas[i].str);
        }
    }
    free(nd->datas);
    free(nd->childs);
}

void saveNode(btree* bt, node* nd)
{
    u8 * bin = bt->buf.win;
    move_window(&bt->buf, nd->nodeNo);
    bt->buf.dirty = true;
    *(u32*)(bin + PARENTBLOCK) = nd->parent;
    *(u32*)(bin + ENTRYCOUNT) = nd->N;
    *(u32*)(bin + NEXTBLOCK) = nd->next;
    for (size_t i = 0; i < nd->N; i++)
    {
        u8* base = bin + bt->nodeSize * i + DATAHEAD;
        switch (bt->type)
        {
        case INT:
            *(int*)base = nd->datas[i].i;
            break;
        case CHAR:
            memcpy((char*)base, nd->datas[i].str, bt->nodeSize - sizeof(u32));
            break;
        case FLOAT:
            *(float*)base = nd->datas[i].f;
            break;
        default: break;
        }
        base += bt->nodeSize - sizeof(u32);
        *(u32*)(base) = nd->childs[i];
    }
    sync_window(&bt->buf);
}

void findNode(btree* bt, node* nd, Data* data)
{
    u32 cur = bt->root;
    getNode(bt, nd, cur);
    while (nd->next == NONLEAFMARK)
    {
        u32 i;
        for (i = 0; i < nd->N && cmp(bt->type, nd->datas[i], *data) <= 0; i++);
        cur = nd->childs[i];
        getNode(bt, nd, cur);
    }
}

void splitNode(btree* bt, node* source, node* dst)
{
    u32 halfcapa = (bt->capacity + 1) / 2;
    u32 mid = bt->capacity + 1 - halfcapa;
    for (size_t i = 0; i < halfcapa; i++)
    {
        insertData(bt, dst, i, &source->datas[mid + i], source->childs[mid + i]);
    }
    source->N = mid;
    dst->N = halfcapa;
    if (source->next!=NONLEAFMARK)
    {
        dst->next = source->next;
        source->next = dst->nodeNo;
    }
    else
    {
        for (size_t i = 0; i < dst->N; i++)
        {
            node tmp;
            getNode(bt, &tmp, dst->childs[i]);
            tmp.parent = dst->nodeNo;
            freeNode(bt, &tmp);
        }
    }

}

u8 cmp(dataType type, Data sourse, Data target)// 1: GT 0:LE
{
    u8 cmp;
    switch (type)
    {
    case INT:
        cmp = sourse.i>target.i ? 1 : 0;
        break;
    case CHAR:
        cmp = strcmp(sourse.str, target.str) ? 1 : 0;
        break;
    case FLOAT:
        cmp = sourse.f > target.f ? 1 : 0;
        break;
    default:
        cmp = 0;
        break;
    }
    return cmp;
}

void insertData(btree* bt,node* nd, u32 index, Data* target, u32 value)
{
    for (size_t i = nd->N; i > index; i--)
    {
        nd->datas[i + 1] = nd->datas[i];
    }
    for (size_t i = nd->N; i > index; i--)
    {
        nd->childs[i + 1] = nd->childs[i];
    }
    nd->datas[index] = *target;
    nd->childs[index] = value;
    nd->N++;
}