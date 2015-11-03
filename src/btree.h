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
/* Public Functions */
bool btree_create(
    const char *,   // index name
    column *col     // column to build index
    );
bool btree_insert(
    const char *,   // index name
    Data* i,         // Data to insert
    u32 value       // value(blocknum)
    );
u32 btree_delete(
    const char *,
    Rule *
    );
u32 btree_delete_node(
    const char *,
    Data *
    );
set<u32> btree_select(
    const char *,   // index name
    Rule *        // Filter    
    );

/* Private */
struct node
{
    u32 parent;   // 4 byte
    /* The first 2 block is to store block info */
    Data *datas=NULL;
    u32 *childs=NULL;
    u32 N;
    u32 nodeNo;     // 4 byte
    u32 next;
};  
typedef struct node node;
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
//static void insert(btree *, node *);
u32 btree_delete_node(
    btree *,   // index name
    Data*         // Filter
    );
static bool getBtree(btree*, const char *);
static void saveBtree(btree*);

static void getNode(btree*,node * ,u32 block);
static void newNode(btree*, node *);
static void freeNode(btree*, node *);
static void saveNode(btree*, node *);

static void findNode(btree*, node*, Data*);
static void splitNode(btree*, node*source,node*dst);
static void mergeNode(btree*, node*source, node*dst);

static i8 cmp(dataType type, Data sourse,Data target);
static void datacpy(btree* bt, Data *dst, Data *source);

static void insertData(btree*,node *nd, u32 index, Data* target, u32 value);
static void insertNonleaf(btree*, node*, u32 parent);

static void deleteData(btree*, node *nd, u32 index);
static void deleteNonleaf(btree*,Data*, u32 parent);
static void refreshBlock(btree*);
static void refreshParents(btree* bt,Data *data,u32 parent,u32 offset);
void travel(const char *);
void travel2(const char*);
#endif // _BTREE_H
