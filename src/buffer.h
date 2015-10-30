#ifndef _BUFFER_H
#define _BUFFER_H
#include "global.h"

/* Buffer Section */

#define CACHESIZE 32 

struct Buffer
{
    u8 win[BLOCKSIZE]; // The window for client to visit
    u32 winptr;// The block cached in win
    bool dirty;
    /* cache layer */
    u8 _cache[CACHESIZE][BLOCKSIZE];
    u32 _cacheptr[CACHESIZE];
    u8 _dirty[CACHESIZE];
    char filename[259];
};
typedef struct Buffer Buffer;
/* Public Functions */
void buffer_init(Buffer *, const char *);
void sync_window(Buffer *);
void move_window(Buffer *,u32);
void newBlock(Buffer *);
/* Private Functions */
static void diskWrite(Buffer *buf, u32 block, u8 * bin);
static void diskRead(Buffer *buf, u32 block, u8 * bin);
#endif // _BUFFER_H
