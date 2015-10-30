#ifndef _BTREE_H
#define _BTREE_H

#include "global.h"
#include "MiniSQL.h"
extern "C"{
    /* Public Functions */
    bool btree_create(
        table *tb,
        u32 index);
    bool btree_insert(
        table *tb,
        u32 index,
        item i
        );
    bool btree_delete(
        table *tb,
        u32 index,
        item i
        );
    /* Private */
    struct node
    {
        u32 keyNum;
        node *parent;
        node **childs;
        Data *datas;
        u32 N;// B+ tree capacity
    };
    typedef struct node node;
    static node *allocNode(u32 N);
    struct btree
    {
        node *head;
    };
    typedef struct btree btree;
    static insert(btree *, )

}
#endif // _BTREE_H
