#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include "MiniSQL.h"
#include "list.h"

static record * binary2record(table *, u8 *);

MiniList *Recordmanager_getRecord(table *tb);   



#endif // _RECORDMANAGER_H

