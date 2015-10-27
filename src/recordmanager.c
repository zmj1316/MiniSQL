#include "recordmanager.h"
#include "buffer.h"
#include <memory.h>
static record * binary2record(table* tb, u8* bin)
{
    record *result = mallocZero(sizeof(record));
    if ((*bin) == 0xFF)
    {
        result->valid = False;
        return result;
    }
    bin++;
    result->valid = True;
    for (u32 i = 0; i < tb->colNum_u64; i++)
    {
        column *col = &(tb->col[i]);
        result->i[i].type = col->type;
        switch (col->type)
        {
        case INT:
            result->i[i].data.i = *(int *)bin;
            break;
        case FLOAT:
            result->i[i].data.f = *(float*)bin;
            break;
        case CHAR:
            result->i[i].data.str = mallocZero(col->size_u8);
            memcpy(result->i[i].data.str,(char *) bin, col->size_u8);/* Mem leap! */
            break;
        default:
            break;
        }
        bin += col->size_u8;
    }
    return result;
}

Bool record2binary(table* tb, u8* bin, record * entry)
{
    *bin++ = 0;/* valid = true */
    for (u32 i = 0; i < tb->colNum_u64; i++)
    {
        column *col = &(tb->col[i]);
        item * it = &(entry->i[i]);
        switch (it->type)
        {
        case INT: 
            *(int*)bin = it->data.i;
            break;
        case CHAR: 
            memcpy(bin, it->data.str, col->size_u8);
            break;
        case FLOAT: 
            *(float*)bin = it->data.f;
            break;
        default:
            break;
        }
    }
    return True;
}

MiniList*                   /* MiniList of record */
Recordmanager_getRecord(
    table* tb           /* table to visit */
    )
{
    MiniList *result;
    result = mallocZero(sizeof(MiniList)); /* Record list handle */
    u32 capacity = BLOCKSIZE / (tb->recordSize + 1); /* Number of records in one block (Record size is +1 for the valid flag)*/
    for (u32 i = 0; i < (tb->recordNum - 1) / capacity + 1; i++) /* For all blocks */
    {
        move_window(&(tb->buf), i);  /* move to operation block */
        for (u32 i = 0; i < capacity; i++)
        {
            u8 *ptr = tb->buf.win;
            List_append(result, binary2record(tb,tb->buf.win + i * tb->recordSize));
        }
    }
    return result;
}

Bool Recordmanager_insertRecord(table* tb, record* entry)
{
    u32 capacity = BLOCKSIZE / (tb->recordSize + 1); /* Number of records in one block (Record size is +1 for the valid flag)*/
    if (tb->recordNum % capacity == 0)
    {
        newBlock(&(tb->buf));
    }
    move_window(&(tb->buf),(++tb->recordNum - 1) / capacity + 1);
    record2binary(tb, tb->buf.win + (tb->recordNum - 1) % capacity, entry);
    tb->buf.dirty = True;
}