#include "recordmanager.h"
#include "buffer.h"
#include <memory.h>
static record * binary2record(table* tb, u8* bin)
{
    u32 ptr = 0;
    record *result = mallocZero(sizeof(record));
    if ((*bin) == 0xFF)
    {
        result->valid = False;
        return result;
    }
    result->valid = True;
    for (u64 i = 0; i < tb->colNum_u64; i++)
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

MiniList*                   /* MiniList of record */
Recordmanager_getRecord(
    table* tb           /* table to visit */
    )
{
    MiniList *result;
    result = mallocZero(sizeof(MiniList)); /* Record list handle */
    u32 capacity = BLOCKSIZE / tb->recordSize; /* Number of records in one block */
    for (u32 i = 0; i < (tb->recordNum + 1) / capacity; i++)
    {
        move_window(tb->buf, i);
        for (u32 i = 0; i < capacity; i++)
        {
            u8 *ptr = tb->buf->win;
            List_append(result, binary2record(tb,tb->buf->win + i * tb->recordSize));
        }
    }
    return result;
}