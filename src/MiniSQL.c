#include "MiniSQL.h"
#include "catalog.h"


int miniSQL_open(
    const char *filename,   /* Database filename  */
    miniSQL **ppDb          /* OUT: MiniSQL db handle */
    )
{
    miniSQL *db;          /* Store allocated handle here */
    int rc = 0;               /* Return code */

    *ppDb = 0;
    db = mallocZero(sizeof(miniSQL)); 
    /*db->buf = getBuffer(filename);*/

    *ppDb = db;
    return rc;
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



MiniList * miniSQL_select(miniSQL *db, table *tb,Filter * filter)
{
    MiniList *result;
    result = mallocZero(sizeof(MiniList));
    return result;
}

Bool miniSQL_insert(miniSQL *db, table *tb, MiniList *record)
{
    return True;
}

Bool miniSQL_delete(miniSQL *db, table *tb, MiniList *record)
{
    return True;
}