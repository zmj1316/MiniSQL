#include "buffer.h"
#include <string.h>
#include <stdio.h>
static void diskWrite(Buffer *buf, u32 block,u8 *bin)
{
    FILE *fp;
    fp = fopen(buf->filename, "rb+");
    fseek(fp, block*BLOCKSIZE, SEEK_SET);
    fwrite(bin, BLOCKSIZE, 1, fp);
    fclose(fp);
}

static void diskRead(Buffer* buf, u32 block, u8* bin)
{
    FILE *fp;
    fp = fopen(buf->filename, "rb");
    fseek(fp, block*BLOCKSIZE, SEEK_SET);
    fread(bin, BLOCKSIZE, 1, fp);
    fclose(fp);
}

void buffer_init(Buffer*buf, const char* filename)
{
    strcpy(buf->filename,filename);
    buf->dirty = False;
}

void sync_window(Buffer* buf)
{
    if (buf->dirty)
    {
        diskWrite(buf, buf->winptr, buf->win);
        buf->dirty = False;
    }
}

void move_window(Buffer* buf, u32 block)
{
    sync_window(buf);
    diskRead(buf, block, buf->win);
}

void newBlock(Buffer* buf)
{
    FILE *fp;
    fp = fopen(buf->filename, "ab");
    fseek(fp, BLOCKSIZE, SEEK_END);
    fclose(fp);
}