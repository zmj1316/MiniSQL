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


struct List
{
    unsigned long size;
    node *head;
    node *it;
    unsigned long itemSize_u32;
};
typedef struct List List;
void List_init(List *, unsigned int);
void List_append(List *, void *);
void *List_it(List *);
void List_delete(List *);
#endif /* _LIST_H */
