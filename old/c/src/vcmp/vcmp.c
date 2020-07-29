/* Programa principal para comparar vacíos de O(n) y O(n log(n)). */

#include <stdlib.h>
#include <stdio.h>
#include "tuple.h"
#include "aux.h"
#include "args.h"
#include "../shared/consts.h"



int main(int argc, char **argv)
{
	Tuple *a;
	Tuple *b;
	
	char *onlogn_path;
	char *on_path;
	
	int onlogn_vnumber;
	int on_vnumber;
	
	int equal_voids;
	
	read_arguments(argc, argv, &onlogn_path, &on_path);
	
	a = read_onlogn_voids(onlogn_path, &onlogn_vnumber);
	b = read_on_voids(on_path, &on_vnumber);
	
	if(onlogn_vnumber != on_vnumber)
	{
		printf("Número de vacíos es distinto: %d y %d.\n", onlogn_vnumber, on_vnumber);
		exit(EXIT_FAILURE);
	}
	
	printf("Verificando %d triángulos...\n", onlogn_vnumber);
	
	equal_voids = compare_voids(a, b, on_vnumber);
	
	if(equal_voids)
	{
		printf("Los vacíos son iguales.\n");
	}
	else
	{
		printf("Los vacíos son distintos.\n");
	}
	
	return EXIT_SUCCESS;
}
