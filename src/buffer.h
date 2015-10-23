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
void *getBuffer(const char *);
void sync_window();
void move_window(u32);

#endif // _BUFFER_H

