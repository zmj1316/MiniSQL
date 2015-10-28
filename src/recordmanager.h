#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include "MiniSQL.h"
#include "list.h"
/* Convert a record in block into a record */
/* Warnning: there's a valid flag before each record binary!*/
static record * binary2record(table *, u8 *);
static Bool record2binary(table *, u8 *, record *);


MiniList *Recordmanager_getRecord(table *tb);   
Bool Recordmanager_insertRecord(table *tb, record *);


#endif // _RECORDMANAGER_H
