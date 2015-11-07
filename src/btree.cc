#include "btree.h"
#include "global.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <io.h>
#include <vector>
using namespace std;
/* create index of {col} in {tb} named {idxname} */
bool btree_create(const char* tablename,const char * idxname, column *col)
{
    if (col == NULL)
    {
        fprintf(stderr, "Column not Exists!\n");
        return false;
    }
    /* generate the index filename */
    char filename[259];
    strcpy(filename,  idxname);
    strcat(filename,".idx");
    /* check the index file */
    if (_access(filename,0)==0)
    {
        fprintf(stderr, "Index Exists Already!\n");
        return false;
    }
    /* buffer for write */
    static Buffer buf;
    buffer_init(&buf, filename);
    /* alloc new block */
    newBlock(&buf);
    /* set index attributes */
    move_window(&buf,0);
    *((u32*)(buf.win + NODESIZE)) = col->size_u8 + sizeof(u32);
    *(u8*)(buf.win + DATATYPE) = col->type;
    *(u32*)(buf.win + BLOCKCOUNT) = 0;
    *(u32*)(buf.win + ROOTPTR) = 1;
    *(u32*)(buf.win + HEADPTR) = 1;
    strcpy((char*)buf.win + TABLENAME, tablename);
    buf.dirty = true;
    sync_window(&buf);
    /* alloc root node */
    static btree bt;
    getBtree(&bt, idxname);
    node n;
    newNode(&bt, &n);
    n.parent = ROOTMARK;
    n.next = NONEXT;
    saveNode(&bt, &n);
    freeNode(&bt, &n);
    saveBtree(&bt);
    return true;
}
/* insert data into index */
bool btree_insert(const char *idxname, Data *target,u32 value)
{
    static btree bt;
    /* get index info */
    if (!getBtree(&bt, idxname)) return false;
    node nd;
    memset(&nd, 0, sizeof(node));
    /* find node to insert */
    findNode(&bt, &nd, target);
    /* find locate inside node*/
    size_t i;
    for (i = 0; i < nd.N && cmp(bt.type, nd.datas[i], *target) <= 0; i++);
    /* perform insert */
    insertData(&bt,&nd, i, target, value);
    saveNode(&bt, &nd);
    /* check if node is overfull */
    if (nd.N>bt.capacity) // split
    {
        /* alloc new node */
        node nnode;
        newNode(&bt, &nnode);
        /* split node into two */
        splitNode(&bt, &nd, &nnode);
        saveNode(&bt, &nd);
        /* insert new node into parent */
        insertNonleaf(&bt, &nnode, nd.parent);
        freeNode(&bt, &nnode);
    }
    freeNode(&bt, &nd);
    saveBtree(&bt);
    return true;
}

const char* btree_getTable(const char* idxname)
{
    /* generate the index filename */
    char filename[259];
    strcpy(filename, idxname);
    strcat(filename, ".idx");
    /* check the index file */
    if (_access(filename, 0) != 0)
    {
        fprintf(stderr, "Index Not Exists!\n");
        return NULL;
    }
    /* buffer for write */
    static Buffer buf;
    buffer_init(&buf, filename);
    /* get index attributes */
    move_window(&buf, 0);
    return (const char*)(buf.win + TABLENAME);
}

u32 btree_delete_node(const char * idxname,Data * data)
{
    static btree bt;
    if (!getBtree(&bt, idxname))
    {
        fprintf(stderr, "Index Not Exist!");
        return 0;
    }
    node nd;
    /* find node to delete */
    findNode(&bt, &nd, data);
    size_t i;
    /* location in node */
    for (i = 0; i < nd.N && cmp(bt.type, nd.datas[i], *data) != 0; i++);
    if (i>=nd.N )
    {
        return 0;
    }
    deleteData(&bt, &nd, i);
    saveNode(&bt, &nd);
    /* less than N/2 */
    if (nd.N<bt.capacity/2 && nd.parent!=ROOTMARK)
    {
        node next;
        if (nd.next!=NONEXT)                // find the next leaf
        {
            getNode(&bt, &next, nd.next);
            if (nd.N+next.N<=bt.capacity)   // merge nodes
            {
                if (nd.nodeNo==bt.head)
                {
                    bt.head = next.nodeNo;  // check if is head
                }
                /* move data */
                for (size_t i = 0; i < nd.N; i++)
                {
                    insertData(&bt, &next, i, &nd.datas[i], nd.childs[i]);
                }
                /* empty node */
                nd.N = EMPTYBLOCK;
                saveNode(&bt, &next);
                saveNode(&bt, &nd);
                deleteNonleaf(&bt, &nd.datas[0], nd.parent);    // delete in parent
                getNode(&bt, &next, nd.next);
                refreshParents(&bt, &next.datas[0],next.parent,0);
                refreshBlock(&bt);
            }
            else    // merge and split
            {
                u32 size = (nd.N + next.N);
                /* move data */
                u32 end = size / 2 - nd.N;
                for (size_t i = 0; i < end; i++)
                {
                    insertData(&bt, &nd, nd.N, &next.datas[0], next.childs[0]);
                    deleteData(&bt, &next, 0);
                }
                nd.N = size / 2;
                next.N = size - (size / 2);
                saveNode(&bt, &nd);
                saveNode(&bt, &next);
                refreshParents(&bt, &next.datas[0], next.parent, -1);
                /* update parent of next */
            }
        }
        else        // end block 
        {
            node par;
            getNode(&bt, &par, nd.parent);                  // get parent
            getNode(&bt, &next, par.childs[par.N - 2]);     // get previous block
            freeNode(&bt, &par);                            // free parent
            if (nd.N + next.N <= bt.capacity)               // merge
            {
                /* move data */
                for (size_t i = 0; i < nd.N; i++)
                {
                    insertData(&bt, &next, next.N, &nd.datas[i], nd.childs[i]);
                }
                nd.N = EMPTYBLOCK;
                saveNode(&bt, &next);
                saveNode(&bt, &nd);
                deleteNonleaf(&bt, &nd.datas[0], nd.parent);// delete in parent
                /* refresh block link */
                refreshBlock(&bt);
            }
            else    // merge and split
            {
                u32 size = (nd.N + next.N);                     
                next.N = size - (size / 2);
                u32 end = size / 2 - nd.N;
                /* move data in to next node */
                for (size_t i = 0; i < end; i++)
                {
                    insertData(&bt,&nd,nd.N,&next.datas[next.N+i],next.childs[next.N+i]);
                }
                nd.N = size / 2;
                /* update self in parent */
                saveNode(&bt, &nd);
                saveNode(&bt, &next);
                refreshParents(&bt, &nd.datas[0], nd.parent,0);
            }
            freeNode(&bt, &par);
        }
        freeNode(&bt, &next);

    }
    freeNode(&bt, &nd);
    saveBtree(&bt);
    return 1;
}

set<u32> btree_select(const char* idxname, Rule * rule)
{
    /* get index info*/
    static btree bt;
    set<u32> res;
    if (!getBtree(&bt, idxname))
    {
        fprintf(stderr, "Index Not Exist!");
        return res;
    }
    node nd;
    u32 head = bt.head;
    switch (rule->cmp)
    {
    case LT: 
    case LE: 
        //head = bt.head; // select form head
        break;
    case EQ:
    case GE:
    case GT: 
        findNode(&bt, &nd, &rule->target);  // select from middle
        head = nd.nodeNo;
        break;
    default: break;
    }
    if (rule->cmp == EQ)        // on EQ case return only one block( or multi-entriy exists just in case ) 
    {
        getNode(&bt, &nd, head);

        for (size_t i = 0; i < nd.N; i++)
        {
            if (Rule_cmp(bt.type, &nd.datas[i], &rule->target, rule) == 1)
            {
                res.insert(nd.childs[i]);
            }
        }
        return res;
    }
    /* other case */
    while (head!=NONEXT)
    {
        getNode(&bt, &nd, head);    // get first leaf node
        for (size_t i = 0; i < nd.N; i++)
        {
            if (Rule_cmp(bt.type,&nd.datas[i],&rule->target,rule)==1)
            {
                res.insert(nd.childs[i]);
            }
            else if(!res.empty())   // have found target and overrun
            {
                freeNode(&bt, &nd);
                return res;
            }
        }
        if (res.empty())    // after searching no result 
        {
            freeNode(&bt, &nd);
            return res;
        }
        head = nd.next; // go to next block
    }
    freeNode(&bt, &nd); // recycle mem
    return res;
}

/* get index attributes */
bool getBtree(btree* bt_ptr, const char* idxname)
{
    /* generate file name */
    char filename[259];
    strcpy(filename,  idxname);
    strcat(filename, ".idx");
    /* check file */
    if (_access(filename, 0) != 0)
    {
        fprintf(stderr, "Index Don't Exists!");
        return false;
    }
    /* initialize buffer */
    buffer_init(&bt_ptr->buf, filename);
    /* read jead block */
    move_window(&(bt_ptr->buf), 0);
    bt_ptr->nodeSize = *(u32 *)(bt_ptr->buf.win + NODESIZE);    // node size
    bt_ptr->type = (dataType)*(bt_ptr->buf.win + DATATYPE);     // data type
    bt_ptr->capacity = (BLOCKSIZE - DATAHEAD) / bt_ptr->nodeSize - 1;   // capacity of one block
    bt_ptr->root = *(u32*)(bt_ptr->buf.win + ROOTPTR);          // root block
    bt_ptr->blockcount = *(u32*)(bt_ptr->buf.win + BLOCKCOUNT); // count of blocks
    bt_ptr->head = *(u32*)(bt_ptr->buf.win + HEADPTR);
    return true;
}

void saveBtree(btree* bt)
{

    /* move to head block */
    move_window(&bt->buf, 0);
    bt->buf.dirty = true;
    /* write attributes */
    *(u32*)(bt->buf.win + BLOCKCOUNT) = bt->blockcount;
    *(u32*)(bt->buf.win + ROOTPTR) = bt->root;
    *(u32*)(bt->buf.win + HEADPTR) = bt->head;
    sync_window(&bt->buf);
}

void getNode(btree* bt, node* nd, u32 block)
{
    /* check range */
    if (block>bt->blockcount)
    {
        fprintf(stderr, "block out of range");
    }
    /* read disk */
    move_window(&bt->buf, block); 
    u8 * bin = bt->buf.win;
    nd->parent = *((u32*)(bin + PARENTBLOCK));
    nd->N = *((u32*)bin + ENTRYCOUNT);
    nd->nodeNo = block;
    nd->next = *(u32*)(bin + NEXTBLOCK);
    /* alloc memory */
    if (nd->childs==NULL)
    {
        nd->childs = new u32[bt->capacity + 1];
    }
    //nd->datas = new Data[bt->capacity + 1];
    if (nd->datas==NULL)
    {
        nd->datas = (Data*)calloc(sizeof(Data), bt->capacity + 1);
    }
    /* assign values */
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
/* alloc new node in new block*/
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

/* recycle node */
void freeNode(btree* bt, node* nd)
{
    if (bt->type==CHAR)
    {
        for (size_t i = 0; i < nd->N; i++)
        {
            free(nd->datas[i].str);
        }
    }
    //free(nd->datas);
    //free(nd->childs);
}

/* save node info */
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

/* find leaf node fits data */
void findNode(btree* bt, node* nd, Data* data)
{
    /* start with root */
    u32 cur = bt->root;
    getNode(bt, nd, cur);
    while (nd->next == NONLEAFMARK) // until find leaf
    {
        u32 i;
        for (i = 1; i < nd->N && cmp(bt->type, nd->datas[i], *data) <= 0; i++);
        cur = nd->childs[i - 1];
        getNode(bt, nd, cur);
    }
}

/* split half of the source node into dst*/
void splitNode(btree* bt, node* source, node* dst)
{
    u32 halfcapa = (bt->capacity + 1) / 2;  // nodes to be moved 
    u32 mid = bt->capacity + 1 - halfcapa;  // nodes remained
    for (size_t i = 0; i < halfcapa; i++)
    {
        insertData(bt, dst, i, &source->datas[mid + i], source->childs[mid + i]);
    }
    source->N = mid;
    dst->N = halfcapa;
    /* assign parent */
    dst->parent = source->parent;
    if (source->next!=NONLEAFMARK)  //leaf nodes
    {
        /* link the nodes */
        dst->next = source->next;
        source->next = dst->nodeNo;
    }
    else                            //nonleaf nodes
    {
        /* assign childs of new node */
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



/* compare two data */
i8 cmp(dataType type, Data sourse, Data target)// 1: GT 0:EQ -1: LT
{
    float cmp;
    switch (type)
    {
    case INT:
        cmp = sourse.i - target.i ;
        break;
    case CHAR:
        cmp = strcmp(sourse.str, target.str);
        break;
    case FLOAT:
        cmp = sourse.f - target.f;
        break;
    default:
        cmp = 0;
        break;
    }
    if (cmp > 0) return 1;
    if (cmp < 0.000001 && cmp > -0.000001) return 0;
    return -1;
}

/* copy one data to another */
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

/* insert data into node */
void insertData(btree* bt,node* nd, u32 index, Data* target, u32 value)
{
    /* only for initial root */
    if (nd->N==0)
    {
        datacpy(bt, &nd->datas[index], target);
        nd->childs[index] = value;
        nd->N++;
        return;
    }
    /* move the data */
    for (size_t i = nd->N; i > index; i--)
    {
        nd->datas[i] = nd->datas[i - 1];
        nd->childs[i] = nd->childs[i - 1];
    }
    /* insert */
    datacpy(bt, &nd->datas[index],target);
    nd->childs[index] = value;
    nd->N++;
}

/* insert into nonleaf node */
void insertNonleaf(btree* bt, node* nd, u32 parent)
{
    if (parent==ROOTMARK)// need new root (root splited)
    {
        node nnode;
        node root;
        newNode(bt, &nnode);            // new root
        getNode(bt, &root, bt->root);   // root
        /* assign new root */
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
    }
    else    // normal
    {
        node p; // parent node to be inserted
        getNode(bt, &p, parent);
        /* locate */
        size_t i;
        for (i = 0; i < p.N && cmp(bt->type, p.datas[i], nd->datas[0]) <= 0; i++);
        insertData(bt, &p, i, &nd->datas[0], nd->nodeNo);
        nd->parent = parent;
        saveNode(bt, nd);
        saveNode(bt, &p);
        if (p.N>bt->capacity) // split non leaf node
        {
            node nnode;                 
            newNode(bt, &nnode);        // new nonleaf node
            nnode.next = NONLEAFMARK;   // mark as nonleaf
            splitNode(bt, &p, &nnode);  // split
            saveNode(bt, &p);           // save inserted node
            insertNonleaf(bt, &nnode, p.parent);    // insert the new node into upper parent
            freeNode(bt, &nnode);
        }
        freeNode(bt, &p);
    }
}
/********************/
/* test functions   */
void travel(const char * str)
{
    static btree bt;
    getBtree(&bt, str);
    u32 cur = bt.head;
    FILE *fp;
    fp = fopen("out.txt", "a");
    node nd;
    while (cur != NONEXT)
    {
        getNode(&bt, &nd, cur);
        for (size_t i = 0; i < nd.N; i++)
        {
            fprintf(fp, "%s\n", nd.datas[i].str);
            //fputs(n.datas[i].str, fp);
        }
        fprintf(fp, "***********\n");
        cur = nd.next;
    }
    fprintf(fp, "========================================\n");
    fclose(fp);
    freeNode(&bt, &nd);
}
void travel2(const char* str)
{
    static btree bt;
    getBtree(&bt, str);
    u32 cur = bt.root;
    FILE *fp;
    fp = fopen("out.txt", "a");
    while (true)
    {
        node nd;
        getNode(&bt, &nd, cur);
        for (size_t i = 0; i < nd.N; i++)
        {
            
        }
    }
}

void deleteData(btree* bt, node* nd, u32 index)
{
    for (size_t i = index; i < nd->N; i++)
    {
        nd->datas[i] = nd->datas[i + 1];
        nd->childs[i] = nd->childs[i + 1];
    }
    nd->N--;
    if (index==0)
    {
        refreshParents(bt, &nd->datas[0], nd->parent,-1);
    }
}

void deleteNonleaf(btree* bt,Data* data, u32 parent)
{
    node nd;
    getNode(bt, &nd, parent);   // node to operate on 
    u32 i;
    /* locate */
    for (i = 0; i < nd.N && cmp(bt->type, nd.datas[i], *data) <= 0; i++);
    deleteData(bt, &nd, i - 1);
    saveNode(bt, &nd);
    if (nd.N<bt->capacity/2)    // too small
    {
        if (nd.parent==ROOTMARK)// root node
        {
            if (nd.N<2)   // delete root 
            {
                node nnd; // new root
                getNode(bt, &nnd, nd.childs[0]);
                nd.N = EMPTYBLOCK;
                nnd.parent = ROOTMARK;
                bt->root = nnd.nodeNo;
                saveNode(bt, &nnd);
                freeNode(bt, &nnd);
                saveNode(bt, &nd);
            }
            return;
        }
        node pd;
        getNode(bt, &pd, nd.parent);        // get parent
        for (i = 0; i < pd.N && cmp(bt->type, pd.datas[i], nd.datas[0]) <= 0; i++);
        node next;
        node tmp;
        if (i<pd.N)     // found next node
        {
            getNode(bt, &next, pd.childs[i]);
            if (nd.N + next.N <= bt->capacity)  // merge 
            {
                for (size_t ii = 0; ii < nd.N; ii++)
                {
                    insertData(bt, &next, ii, &nd.datas[ii], nd.childs[ii]);
                    getNode(bt, &tmp, nd.childs[ii]);
                    tmp.parent = next.nodeNo;
                    saveNode(bt, &tmp);
                }
                nd.N = EMPTYBLOCK;
                saveNode(bt, &next);
                saveNode(bt, &nd);
                deleteNonleaf(bt, &nd.datas[0], nd.parent);
                getNode(bt, &next, pd.childs[i]);
                refreshParents(bt, &next.datas[0], next.parent,0);
            }
            else        // merge and split
            {
                u32 size = (nd.N + next.N);
                u32 end = size / 2 - nd.N;
                for (size_t i = 0; i < end; i++)
                {
                    insertData(bt, &nd, nd.N, &next.datas[0], next.childs[0]);
                    getNode(bt, &tmp, next.childs[0]);
                    tmp.parent = nd.nodeNo;
                    saveNode(bt, &tmp);
                    deleteData(bt, &next, 0);
                }
                nd.N = size / 2;
                next.N = size - (size / 2);
                /* update parent */
                saveNode(bt, &nd);
                saveNode(bt, &next);
                refreshParents(bt, &next.datas[0], next.parent, -1);
            }
        }
        else        // end node
        {
            getNode(bt, &next, pd.childs[pd.N - 2]);     // get previous block
            if (nd.N + next.N <= bt->capacity)               // merge
            {
                /* move data */
                for (size_t i = 0; i < nd.N; i++)
                {
                    insertData(bt, &next, next.N, &nd.datas[i], nd.childs[i]);
                    getNode(bt, &tmp, nd.childs[i]);
                    tmp.parent = next.nodeNo;
                    saveNode(bt, &tmp);
                }
                nd.N = EMPTYBLOCK;
                saveNode(bt, &next);
                saveNode(bt, &nd);
                deleteNonleaf(bt, &nd.datas[0], nd.parent);// delete in parent
                /* refresh block link */
                refreshBlock(bt);
            }
            else    // merge and split
            {
                u32 size = (nd.N + next.N);
                next.N = size - (size / 2);
                /* move data */
                u32 end = size / 2 - nd.N;
                for (size_t i = 0; i < end; i++)
                {
                    insertData(bt, &nd, nd.N, &next.datas[next.N + i], next.childs[next.N + i]);
                    getNode(bt, &tmp, next.childs[next.N + i]);
                    tmp.parent = nd.nodeNo;
                    saveNode(bt, &tmp);
                }
                nd.N = size / 2;
                /* update self in parent */
                saveNode(bt, &nd);
                saveNode(bt, &next);
                refreshParents(bt, &nd.datas[0], nd.parent,0);
            }
        }
        freeNode(bt, &pd);
        freeNode(bt, &next);
        freeNode(bt, &tmp);
    }
    freeNode(bt, &nd);
}

void refreshBlock(btree* bt)
{
    node n0;
    node n1;
    getNode(bt, &n0, bt->head);
    while (n0.next!=NONEXT)
    {
        getNode(bt, &n1, n0.next);
        if (n1.N==0)
        {
            n0.next = n1.next;
            saveNode(bt,&n0);
            if (n0.next==NONEXT)
            {
                break;
            }
        }
        getNode(bt, &n0, n0.next);  
    }
    freeNode(bt, &n0);
    freeNode(bt, &n1);
}

void refreshParents(btree* bt, Data* data, u32 parent,u32 offset)
{
    if (parent==ROOTMARK)
    {
        return;
    }
    node nd;
    getNode(bt, &nd, parent);
    size_t i;
    for (i = 0; i < nd.N && cmp(bt->type, nd.datas[i], *data) <= 0; i++);
    nd.datas[i + offset] = *data;
    saveNode(bt, &nd);
    if (i + offset == 0)
    {
        refreshParents(bt, data, nd.parent,offset);
    }
}
