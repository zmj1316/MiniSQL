/*
* (C) 2015 Key Zhang
* @list.h
* @Created on 2015/10/21
*/
#ifndef _LIST_H
#define _LIST_H
struct _node;
typedef struct _node node;
struct _node
{
    node *Next;
    void *data;
};


struct MiniList
{
    unsigned long size;
    node *head;
    node *it;
    unsigned long itemSize_u32;
};
typedef struct MiniList MiniList;
void List_init(MiniList *, unsigned int);
void List_append(MiniList *, void *);
void *List_it(MiniList *);
void List_delete(MiniList *);
#endif /* _LIST_H */
