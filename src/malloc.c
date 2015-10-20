#include "global.h"
#include <stdlib.h>
#include <memory.h>
void *mallocZero(u64 n){
	void *p = malloc(n);
	if (p)
	{
		memset(p, 0, n);
	}
	return p;
}