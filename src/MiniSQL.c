#include "global.h"
#include "MiniSQL.h"
#include "catalog.h"


int miniSQL_open(
    const char *filename,   /* Database filename  */
    miniSQL **ppDb          /* OUT: MiniSQL db handle */
    )
{
    miniSQL *db;          /* Store allocated handle here */
    int rc;               /* Return code */

    *ppDb = 0;
    db = mallocZero(sizeof(miniSQL)); 
    db->buf = getBuffer(filename);

    *ppDb = db;
}

int miniSQL_createTable(
    miniSQL *db,      /* Database handle */
    table *tb         /* Table handle */
    )
{
    int rc;
    rc = catalog_createTable(db, tb);
    return rc;
}

int miniSQL_dropTable(
    miniSQL *db,          /* Database handle*/
    const char* tb        /* Table name*/
    )
{
    
}

List * miniSQL_select(miniSQL *db, Filter * filter)
{
    
}