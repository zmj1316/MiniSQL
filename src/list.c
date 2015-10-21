#include "list.h"
#include <stdlib.h>

void List_init(List * list, unsigned int itemSize)
{
    if (list == NULL)
    {
        return;
    }
    list->size = 0;
    list->head = NULL;
    list->it = NULL;
    list->itemSize_u32 = itemSize;
}

void List_append(List * list, void * item)
{
    unsigned int size;
    node *newNode;
    if (list == NULL)
    {
        return;
    }
    size = list->itemSize_u32;
    newNode = malloc(sizeof(node));
    newNode->Next = list->head;
    newNode->data = malloc(list->size);
    memcpy(newNode->data, item, size);
    list->head = newNode;
    list->size++;
}

void *List_it(List * list)
{
    node * tmp = list->it;
    if (list == NULL)
    {
        return NULL;
    }
    list->it = tmp->Next;
    if (tmp == NULL)
    {
        list->it = list->head;
        return NULL;
    }
    return tmp->data;
}

void List_delete(List * list)
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