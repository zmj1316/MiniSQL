#ifndef _BUFFER_H
#define _BUFFER_H
/* Buffer Section */

#define CACHESIZE 32// The cache size of each buffer
struct Buffer
{
    u8 win[BLOCKSIZE]; // The window for client to visit
    u32 winptr;// The block cached in win
    u8 dirty;
    u8 _cache[CACHESIZE][BLOCKSIZE];
    u32 _cacheptr[CACHESIZE];
    u8 _dirty[CACHESIZE];
};
typedef struct Buffer Buffer;
/* Public Functions */
void buffer_init(Buffer *, const char *);
void sync_window(Buffer *);
void move_window(Buffer *,u32);

/* Private Functions */
#endif // _BUFFER_H

