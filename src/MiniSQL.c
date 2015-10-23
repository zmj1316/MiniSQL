#include "MiniSQL.h"
#include "catalog.h"
#include "recordmanager.h"
#include <stdlib.h>

int miniSQL_open(
    const char *filename,   /* Database filename  */
    miniSQL **ppDb          /* OUT: MiniSQL db handle */
    )
{
    miniSQL *db;          /* Store allocated handle here */
    int rc;               /* Return code */

    *ppDb = 0;
    db = mallocZero(sizeof(miniSQL)); 
    /*db->buf = getBuffer(filename);*/

    *ppDb = db;
}

Bool miniSQL_createTable(
    miniSQL *db,      /* Database handle */
    table *tb         /* Table handle */
    )
{
    Bool rc;
    rc = catalog_createTable(db, tb);
    return rc;
}

Bool miniSQL_dropTable(
    miniSQL *db,          /* Database handle*/
    const char* tb        /* Table name*/
    )
{
    return True;
}



List * miniSQL_select(miniSQL *db, table *tb,Filter * filter)
{
    List *result;
    result = mallocZero(sizeof(List));
    return result;
}

Bool miniSQL_insert(miniSQL *db, table *tb, List *record)
{
    return True;
}

Bool miniSQL_delete(miniSQL *db, table *tb, List *record)
{
    return True;
}