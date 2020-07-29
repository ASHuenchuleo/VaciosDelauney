/* Funciones para operar con tuplas. */

#include "tuple.h"



/* compare_tuple_r
 * 
 * Toma dos tuplas y las compara de acuerdo a su primera
 * componente, r.
 * */

int compare_tuple_r(const void *t, const void *u)
{
	int r0 = (*(const Tuple *)t).r;
	int r1 = (*(const Tuple *)u).r;
	
	if (r0 < r1)
	{
		return -1;
	}
	else if (r0 > r1)
	{
		return 1;
	}
	
	return 0;
}



/* compare_tuple_t
 * 
 * Toma dos tuplas y las compara de acuerdo a su segunda
 * componente, t.
 * */

int compare_tuple_t(const void *t, const void *u)
{
	int t0 = (*(const Tuple *)t).t;
	int t1 = (*(const Tuple *)u).t;
	
	if (t0 < t1)
	{
		return -1;
	}
	else if (t0 > t1)
	{
		return 1;
	}
	
	return 0;
}
