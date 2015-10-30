#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include "MiniSQL.h"
/* Convert a record in block into a record */
/* Warnning: there's a valid flag before each record binary!*/
static record binary2record(table *, u8 *);
static bool record2binary(table *, u8 *, record *);


vector<record> Recordmanager_getRecord(table *tb);   
bool Recordmanager_insertRecord(table *tb, record *);


#endif // _RECORDMANAGER_H
