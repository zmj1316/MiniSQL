#ifndef _CATALOG_H
#define _CATALOG_H

bool catalog_createTable(       // generate the catalog file according to the given property 
    table *
    );
bool catalog_connectTable(      // fill the table property according to the tablename
    table *
    );
bool catalog_dropTable(         // delete the catalog file
    table *
    );
bool catalog_disconnectTable(
    table *
    );
#endif
