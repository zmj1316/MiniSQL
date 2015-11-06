#ifndef _CATALOG_H
#define _CATALOG_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "MiniSQL.h"
#include <io.h>

class temp
{
public:
    char name_str[255];// table name
    column col[MAXCOLUMN];// columns
    temp *next;
};

bool catalog_createTable(       // generate the catalog file according to the given property 
    table *tb
    )
{
    FILE *fp;
    
    char filename[260];
    strcpy(filename,tb->name_str);
    strcat(filename,".cat");
    
    if((fp = fopen(filename,"wb"))==NULL)
    {        fprintf(stderr,"Can't open the file") ;
             return false;
    }
    fwrite(tb->name_str,255,1,fp); 
    fwrite(tb->col,sizeof(column)*MAXCOLUMN,1,fp); 
    fwrite(&tb->colNum_u64,sizeof(u64),1,fp);// column number
    fwrite(&tb->primarykey_u8,sizeof(u8),1,fp); 
    fwrite(&tb->recordSize,sizeof(u32),1,fp); 
    fwrite(&tb->recordNum,sizeof(u32),1,fp); 

    fclose(fp);
    return true;
}
           
bool catalog_connectTable(      // fill the table property according to the tablename
    table *tb
    )
{
    FILE *fp;
    
    char filename[260];
    strcpy(filename,tb->name_str);
    strcat(filename,".cat");
    
    if((fp = fopen(filename,"rb"))==NULL)
    {        fprintf(stderr,"Can't open the file") ;
             return false;
    }

    fread(tb->name_str,255,1,fp); 
    fread(tb->col,sizeof(column)*MAXCOLUMN,1,fp);
    fread(&tb->colNum_u64,sizeof(u64),1,fp);// column number
    fread(&tb->primarykey_u8,sizeof(u8),1,fp); 
    fread(&tb->recordSize,sizeof(u32),1,fp); 
    fread(&tb->recordNum,sizeof(u32),1,fp); 
        
    fclose(fp); 
    return true;    
}

bool catalog_dropTable(         // delete the catalog file
    table *tb
    )
{
    FILE *fp;
    
    char filename[260];
    strcpy(filename,tb->name_str);
    strcat(filename,".cat");
    
    if (_access(filename, 0) != 0)
    {
        fprintf(stderr, "File %s not Exists!\n", filename);
        return false;
    }
    remove(filename);
    return 1;
}

bool catalog_disconnectTable(table *tb)
{
    FILE *fp;
    char filename[260];
    strcpy(filename, tb->name_str);
    strcat(filename, ".cat");
    if ((fp = fopen(filename, "wb")) == NULL)
    {
        fprintf(stderr, "Can't open the file");
        return false;
    }
    fwrite(tb->name_str, 255, 1, fp);
    fwrite(tb->col, sizeof(column)*MAXCOLUMN, 1, fp);
    fwrite(&tb->colNum_u64, sizeof(u64), 1, fp);// column number
    fwrite(&tb->primarykey_u8, sizeof(u8), 1, fp);
    fwrite(&tb->recordSize, sizeof(u32), 1, fp);
    fwrite(&tb->recordNum, sizeof(u32), 1, fp);

    fclose(fp);
    return true;
}

#endif
