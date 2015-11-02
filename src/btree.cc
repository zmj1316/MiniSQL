#include "btree.h"
#include "global.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <io.h>
/* create index of {col} in {tb} named {idxname}*/
bool btree_create(const char * idxname, column *col)
{
    if (col == NULL)
    {
        fprintf(stderr, "Column not Exists!");
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
    static Buffer buf;
    buffer_init(&buf, filename);
    newBlock(&buf);
    move_window(&buf,0);
    *((u32*)(buf.win + NODESIZE)) = col->size_u8 + sizeof(u32);
    *(u8*)(buf.win + DATATYPE) = col->type;
    *(u32*)(buf.win + BLOCKCOUNT) = 0;
    *(u32*)(buf.win + ROOTPTR) = 1;
    buf.dirty = true;
    sync_window(&buf);
    static btree bt;
    getBtree(&bt, idxname);
    node n;
    newNode(&bt, &n);
    n.parent = ROOTMARK;
    n.next = NONEXT;
    saveNode(&bt, &n);
    freeNode(&bt, &n);
    freeBtree(&bt);
    return true;
}

bool btree_insert(const char *idxname, Data *target,u32 value)
{
    static btree bt;
    if (!getBtree(&bt, idxname)) return false;
    node nd;
    findNode(&bt, &nd, target);
    size_t i;
    for (i = 0; i < nd.N && cmp(bt.type, nd.datas[i], *target) <= 0; i++);
    insertData(&bt,&nd, i, target, value);
    saveNode(&bt, &nd);
    if (nd.N>bt.capacity) // split
    {
        node nnode;
        newNode(&bt, &nnode);
        splitNode(&bt, &nd, &nnode);
        saveNode(&bt, &nd);
        insertNonleaf(&bt, &nnode, nd.parent);
        freeNode(&bt, &nnode);
    }
    freeNode(&bt, &nd);
    freeBtree(&bt);
    return true;
}

vector<u32> btree_select(const char* idxname, Rule * rule)
{
    static btree bt;
    vector<u32> res;
    if (!getBtree(&bt, idxname))
    {
        fprintf(stderr, "Index Not Exist!");
        return res;
    }
    node nd;
    u32 head = 1;
    switch (rule->cmp)
    {
    default:
        break;
    case LT: 
    case LE: 
        head = 1;
        break;
    case EQ:
    case GE:
    case GT: 
        findNode(&bt, &nd, &rule->target);
        head = nd.nodeNo;
        break;
    }
    while (head!=NONEXT)
    {
        getNode(&bt, &nd, head);
        for (size_t i = 0; i < nd.N; i++)
        {
            if (Rule_cmp(bt.type,&nd.datas[i],&rule->target,rule)==1)
            {
                res.push_back(nd.childs[i]);
            }
            else
            {
                break;
            }
        }
        head = nd.next;
    }
    return res;

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
    *(u32*)(bt->buf.win + ROOTPTR) = bt->root;
    sync_window(&bt->buf);
}

void getNode(btree* bt, node* nd, u32 block)
{
    if (block>bt->blockcount)
    {
        fprintf(stderr, "block out of range");

    }
    move_window(&bt->buf, block); 
    u8 * bin = bt->buf.win;
    nd->parent = *((u32*)(bin + PARENTBLOCK));
    nd->N = *((u32*)bin + ENTRYCOUNT);
    nd->nodeNo = block;
    nd->next = *(u32*)(bin + NEXTBLOCK);
    if (nd->childs==NULL)
    {
        nd->childs = new u32[bt->capacity + 1];
    }
    //nd->datas = new Data[bt->capacity + 1];
    if (nd->datas==NULL)
    {
        nd->datas = (Data*)calloc(sizeof(Data), bt->capacity + 1);
    }
    for (size_t i = 0; i < nd->N; i++)
    {
        u8* base = bin + bt->nodeSize * i + DATAHEAD;
        switch (bt->type)
        {
        case INT: 
            nd->datas[i].i = *(int*)base;
            break;
        case CHAR: 
            if (nd->datas[i].str==NULL)
            {
                nd->datas[i].str = new char[bt->nodeSize - sizeof(u32)];
            }
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
    newBlock(&bt->buf);
    nd->N = 0;
    nd->nodeNo = bt->blockcount;
    nd->next = NONEXT;
    nd->childs = new u32[bt->capacity + 1];
    nd->datas = new Data[bt->capacity + 1];
}

void freeNode(btree* bt, node* nd)
{
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
    move_window(&bt->buf, nd->nodeNo);
    u8 * bin = bt->buf.win;
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
            strcpy((char*)base, nd->datas[i].str);
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
        for (i = 1; i < nd->N && cmp(bt->type, nd->datas[i], *data) <= 0; i++)
        {
            
        }
        cur = nd->childs[i - 1];
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
    dst->parent = source->parent;
    if (source->next!=NONLEAFMARK)
    {
        //fprintf(stdout, "leaf sp %u to %u to %u\n", source->nodeNo,dst->nodeNo,source->next);
        dst->next = source->next;
        source->next = dst->nodeNo;
    }
    else
    {
        //fprintf(stdout, "nonleaf %u to %u\n", source->nodeNo, dst->nodeNo);

        for (size_t i = 0; i < dst->N; i++)
        {
            node tmp;
            getNode(bt, &tmp, dst->childs[i]);
            tmp.parent = dst->nodeNo;
            saveNode(bt, &tmp);
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
        cmp = strcmp(sourse.str, target.str)>0 ? 1 : 0;
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

void datacpy(btree* bt, Data* dst, Data* source)
{
    switch (bt->type)
    {
    case CHAR:
        (*dst).str = new char[bt->nodeSize - sizeof(u32)];
        strcpy((*dst).str, (*source).str);
        break;
    default:
        *dst = *source;
        break;
    }
}

void insertData(btree* bt,node* nd, u32 index, Data* target, u32 value)
{
    if (nd->N==0)
    {
        datacpy(bt, &nd->datas[index], target);
        nd->childs[index] = value;
        nd->N++;
        return;
    }
    for (size_t i = nd->N; i > index; i--)
    {
        nd->datas[i] = nd->datas[i - 1];
        nd->childs[i] = nd->childs[i - 1];
    }
    datacpy(bt, &nd->datas[index],target);
    nd->childs[index] = value;
    nd->N++;
}

void insertNonleaf(btree* bt, node* nd, u32 parent)
{
    if (parent==ROOTMARK)// new root node
    {
        node nnode;
        node root;
        newNode(bt, &nnode);
        getNode(bt, &root, bt->root);
        nnode.parent = ROOTMARK;
        nnode.N = 2;
        nnode.next = NONLEAFMARK;
        datacpy(bt, &nnode.datas[0], &root.datas[0]);
        nnode.childs[0] = root.nodeNo;
        datacpy(bt, &nnode.datas[1], &nd->datas[0]);
        nnode.childs[1] = nd->nodeNo;
        root.parent = nnode.nodeNo;
        nd->parent = nnode.nodeNo;
        bt->root = nnode.nodeNo;
        saveNode(bt, nd);
        saveNode(bt, &root);
        freeNode(bt, &root);
        saveNode(bt, &nnode);
        freeNode(bt, &nnode);
        //fprintf(stdout, "Root is %u\n", bt->root);
    }
    else
    {
        node p;
        getNode(bt, &p, parent);
        size_t i;
        for (i = 0; i < p.N && cmp(bt->type, p.datas[i], nd->datas[0]) <= 0; i++)
        {
            
        }
        insertData(bt, &p, i, &nd->datas[0], nd->nodeNo);
        nd->parent = parent;
        saveNode(bt, nd);
        saveNode(bt, &p);
        if (p.N>bt->capacity) // split non leaf node
        {
            node nnode;
            newNode(bt, &nnode);
            nnode.next = NONLEAFMARK;
            splitNode(bt, &p, &nnode);
            saveNode(bt, &p);
            insertNonleaf(bt, &nnode, p.parent);
            freeNode(bt, &nnode);
        }
        freeNode(bt, &p);
    }
}
void travel(const char * str)
{
    static btree bt;
    getBtree(&bt, str);
    u32 cur = 1;
    FILE *fp;
    fp = fopen("out.txt", "a");
    while (cur != NONEXT)
    {
        node n;
        getNode(&bt, &n, cur);
        for (size_t i = 0; i < n.N; i++)
        {
            fprintf(fp, "%s\n", n.datas[i].str);
            //fputs(n.datas[i].str, fp);
        }
        fprintf(fp, "***********\n");
        cur = n.next;
    }
    fprintf(fp, "========================================\n");
    fclose(fp);
}

void deleteData(btree* bt, node* nd, u32 index)
{
    for (size_t i = index; i < nd->N; i++)
    {
        nd->datas[i] = nd->datas[i + 1];
        nd->childs[i] = nd->childs[i + 1];
    }
    nd->N--;
}

void deleteNonleaf(btree*, u32 parent)
{

}