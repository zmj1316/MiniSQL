/*
* (C) 2015 Key Zhang
* @btree.h
* @Created on 2015/10/19
*/
#ifndef _BTREE_H
#define _BTREE_H

#include "global.h"
#include "MiniSQL.h"
//offset in head block
#define BLOCKCOUNT      0 // 4byte: block number in index
#define NODESIZE        4 // 4byte: the size of one node
#define ROOTPTR         8 // 4byte: Root block
#define HEADPTR         12// 4byte: head block
#define DATATYPE        16// 1byte: datatype 0:int 2:float 1:char
#define TABLENAME       17// 256byte: table name
//offset in block
#define ENTRYCOUNT      0 // 4byte: count the number of entries in one block 0xFFFFFFFF >> empty block
#define PARENTBLOCK     4 // 4byte: the parent block number (0xFFFFFFFF for root)
#define NEXTBLOCK       8 // 4byte: the next block number (0xFFFFFFFE for non leaf 0xFFFFFFFF for leaf without next block)
#define DATAHEAD        12// begin of node data


#define ROOTMARK 0xFFFFFFFF
#define NONLEAFMARK 0xFFFFFFFE
#define NONEXT 0xFFFFFFFF
#define EMPTYBLOCK 0x0
#include <set>
#include <io.h>
using namespace std;
/********************/
/* Public Functions */
/********************/

/* create index on table */
bool btree_create(  
    const char* tablename,  // table name
    const char *,           // index name
    column *col             // column to build index
    );
/* insert data into index */
bool btree_insert(
    const char *,        // index name
    Data* i,             // Data to insert
    u32 value            // value(blocknum)
    );
/* get tablename from index name */
const char* btree_getTable( // NULL: index not exist
    const char* idxname     // index name
);
/* delete a data entry in index */
u32 btree_delete_node(      // 0: not deleted 1:deleted
    const char *,           // index name
    Data *                  // data to delete (for once)
    );
set<u32> btree_select(
    const char *,   // index name
    Rule *        // Filter    
    );

/***********/
/* Private */
class node
{
public:
    u32 parent;   // 4 byte
    /* The first 2 block is to store block info */
    Data *datas;
    u32 *childs;
    u32 N;
    u32 nodeNo;     // 4 byte
    u32 next;
    node()
    {
        datas = NULL;
        childs = NULL;
    }
};  
//typedef struct node node;
//static node *allocNode(u32 N);
struct btree
{
    Buffer buf;
    dataType type;
    u8 nodeSize;
    u16 capacity;
    u32 root;
    u32 blockcount;
    u32 head;
};
typedef struct btree btree;

static bool getBtree(btree*, const char *);                 // get btree from index name
static void saveBtree(btree*);                              // save btree info

static void getNode(btree*,node * ,u32 block);              // get node from blocknum
static void newNode(btree*, node *);                        // alloc new node
static void freeNode(btree*, node *);                       // recycle mem
static void saveNode(btree*, node *);                       // save node info

static void findNode(btree*, node*, Data*);                 // find node through btree by data
static void splitNode(btree*, node*source,node*dst);        // split node into 2 by half

static i8 cmp(dataType type, Data sourse,Data target);      // compare 2 data in 3type
static void datacpy(btree* bt, Data *dst, Data *source);    // copy data

// insert data into node by index
static void insertData(btree*,node *nd, u32 index, Data* target, u32 value);
static void insertNonleaf(btree*, node*, u32 parent);

static void deleteData(btree*, node *nd, u32 index);
static void deleteNonleaf(btree*,Data*, u32 parent);
static void refreshBlock(btree*);
static void refreshParents(btree* bt,Data *data,u32 parent,u32 offset);

#endif // _BTREE_H
