#include "list.h"
#include <stdlib.h>
#include "global.h"

void List_init(MiniList * list, unsigned int itemSize)
{
    if (list == NULL)
    {
        return;
    }
    list->size = 0;
    list->head = NULL;
    list->it = list->head;
    list->itemSize_u32 = itemSize;
}

void List_append(MiniList * list, void * item)
{
    node *newNode;
    if (list == NULL || item == NULL)
    {
        return;
    }
    newNode = mallocZero(sizeof(node));
    newNode->Next = list->head;
    newNode->data = item;
    list->head = newNode;
    list->size++;
}

void *List_it(MiniList * list)
{
    if (list == NULL)
    {
        return NULL;
    }
    node * tmp = list->it;
    if (tmp == NULL)
    {
        list->it = list->head;
        return NULL;
    }
    list->it = tmp->Next;
    return tmp->data;
}

void List_delete(MiniList * list)
{
    node *tmp;
    if (list == NULL)
    {
        return;
    }
    while (list->head != NULL)
    {
        tmp = list->head;
        list->head = tmp->Next;
        free(tmp->data);
        free(tmp);
    }
    free(list);
}
