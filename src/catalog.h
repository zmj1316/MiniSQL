#ifndef _CATALOG_H
#define _CATALOG_H
#include "global.h"

Bool catalog_createTable(       // generate the catalog file according to the given property 
    table *
    );
Bool catalog_connectTable(      // fill the table property according to the tablename
    table *
    );
Bool catalog_dropTable(         // delete the catalog file
    table *
    );

#endif
