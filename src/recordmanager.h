#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include"MiniSQL.h"
#include"global.h"

bool Recordmanager_insertRecord(
    table* tab,
    record* rcd
    );


int Recordmanager_deleteRecord(
    table* tab,
    Filter *flt
    );



vector<record> Recordmanager_selectRecord(
    table* tab,
    Filter* flt
    );


bool Recordmanager_addindex(
    table* tb,
    const char * idxname,
    u32 colidx
    );
#endif