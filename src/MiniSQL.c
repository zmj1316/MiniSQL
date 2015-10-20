#include "global.h"
#include "MiniSQL.h"

int miniSQL_open(
  const char *filename,   /* Database filename  */
  miniSQL **ppDb          /* OUT: MiniSQL db handle */
){
    miniSQL *db;          /* Store allocated handle here */
    int rc;               /* Return code */

    *ppDb = 0;
    db = mallocZero(sizeof(miniSQL));
    db->buf = getBuffer(filename);
}