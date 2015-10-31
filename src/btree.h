#ifndef _BTREE_H
#define _BTREE_H

#include "global.h"
#include "MiniSQL.h"
//offset in block
#define ENTRYCOUNT      0 // 4byte: count the number of entries in one block
#define PARENTBLOCK     4 // 4byte: the parent block number (0xFFFFFFFF for root)
#define NEXTBLOCK       8 // 4byte: the next block number (0xFFFFFFFE for non leaf 0xFFFFFFFF for leaf without next block)
#define NODESIZE        12// 4byte: the size of one node
#define DATATYPE        16// 1byte: datatype 0:int 2:float 1:char
#define DATAHEAD        17// begin of node data
#define ROOTMARK 0xFFFFFFFF
#define NONLEAFMARK 0xFFFFFFFE
#define NONEXT 0xFFFFFFFF

/* Public Functions */
bool btree_create(
    table *tb,
    const char *,
    column *col);
bool btree_insert(
    const char *,
    item i,
    u32 value
    );
bool btree_delete(
    table *tb,
    u32 index,
    item i
    );
/* Private */
struct node
{
    u32 parent;   // 4 byte
    /* The first 2 block is to store block info */
    Data *datas;
    u32 *childs;
    u32 N;
    u32 nodeNo;     // 4 byte
};  
typedef struct node node;
//static node *allocNode(u32 N);
struct btree
{
    Buffer buf;
    dataType type;
    u8 nodeSize;
    u16 capacity;
};
typedef struct btree btree;
//static void insert(btree *, node *);
static void getBtree(btree*, const char *);
static void getNode(btree*,node * ,u32 block);
static void saveNode(btree*, node *);
static void shiftData(Data* data, u32 index,u32 N);
static void shiftChild(u32* child, u32 index, u32 N);
#endif // _BTREE_H
