#include "global.h"
#include <stdlib.h>
#include <memory.h>
/* malloc objects filled with zero */
void *mallocZero(u64 n){
	void *p = malloc(n);
	if (p)
	{
		memset(p, 0, n);
	}
	return p;
}
