#ifndef _BUFFER_H
#define _BUFFER_H
#include "global.h"

/* Buffer Section */

#define CACHESIZE 256

struct Buffer
{
#ifdef TINYBUF
    u8 win[BLOCKSIZE]; // The window for client to visit
#else
    u8* win;
    /* cache layer */
    u8 _cache[CACHESIZE][BLOCKSIZE];
    u32 _cacheptr[CACHESIZE];
    bool _dirty[CACHESIZE];
#endif
    u32 winptr;// The block cached in win
    bool dirty;
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
