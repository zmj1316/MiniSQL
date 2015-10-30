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
        node *parent;   // 4 byte
        node *childs[BLOCKSIZE/];  // 
        Data datas[BLOCKSIZE/sizeof(Data)];
        u32 N;// B+ tree capacity 4 byte
        u32 nodeNo;     // 4 byte
    };  
    typedef struct node node;
    static node *allocNode(u32 N);
    struct btree
    {
        node *head;
        u32 N;
    };
    typedef struct btree btree;
    static void insert(btree *, node *);

}
#endif // _BTREE_H
