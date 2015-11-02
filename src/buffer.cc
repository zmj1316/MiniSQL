#include "buffer.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
static void diskWrite(Buffer *buf, u32 block,u8 *bin)
{
    FILE *fp;
    fopen_s(&fp,buf->filename, "rb+");
    fseek(fp, block*BLOCKSIZE, SEEK_SET);
    fwrite(bin, BLOCKSIZE, 1, fp);
    fclose(fp);
}

static void diskRead(Buffer* buf, u32 block, u8* bin)
{
    FILE *fp;
    fopen_s(&fp,buf->filename, "rb");
    fseek(fp, block*BLOCKSIZE, SEEK_SET);
    fread(bin, BLOCKSIZE, 1, fp);
    fclose(fp);
}

void buffer_init(Buffer*buf, const char* filename)
{
    strcpy_s(buf->filename,255,filename);
    buf->dirty = false;
    buf->winptr = -1;
    for (size_t i = 0; i < CACHESIZE; i++)
    {
        buf->_dirty[i] = false;
        buf->_cacheptr[i] = -1;
    }
}
void sync_window(Buffer* buf)
{
    if (buf->dirty)
    {
        diskWrite(buf, buf->winptr, buf->win);
        buf->dirty = false;
    }
}
void move_window(Buffer* buf, u32 block)
{
    if (buf->winptr == block) return;
    sync_window(buf);
    diskRead(buf, block, buf->win);
    buf->winptr = block;
}
//void sync_window(Buffer* buf)
//{
//    if (buf->dirty)
//    {
//        diskWrite(buf, buf->_cacheptr[buf->winptr], buf->win);
//        buf->dirty = false;
//    }
//    for (size_t i = 0; i < CACHESIZE; i++)
//    {
//        if (buf->_dirty[i])
//        {
//            diskWrite(buf, buf->_cacheptr[i], buf->_cache[i]);
//            buf->_dirty[i] = false;
//        }
//    }
//}
//
//void move_window(Buffer* buf, u32 block)
//{
//    if (buf->_cacheptr[buf->winptr] == block) return;
//    sync_window(buf);
//    for (size_t i = 0; i < CACHESIZE; i++)
//    {
//        if (buf->_cacheptr[i]==block)
//        {
//            memcpy(buf->win, buf->_cache[i],BLOCKSIZE);
//            buf->winptr = i;
//            return;
//        }
//    }
//    u32 dst = block%CACHESIZE;
//    diskRead(buf, block, buf->_cache[dst]);
//    buf->_cacheptr[dst] = block;
//    memcpy(buf->win, buf->_cache[dst], BLOCKSIZE);
//    buf->winptr = dst;
//}

void newBlock(Buffer* buf)
{
    FILE *fp;
    fopen_s(&fp,buf->filename, "ab");
    fseek(fp, BLOCKSIZE, SEEK_END);
    fclose(fp);
}
