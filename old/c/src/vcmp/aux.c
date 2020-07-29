/* Funciones auxiliares para leer y comparar vacíos. */

#include <stdlib.h>
#include <stdio.h>
#include "tuple.h"
#include "../shared/consts.h"



/* read_onlogn_voids
 * 
 * Lee un archivo .triangles de la implementación O(n log(n)).
 * */

Tuple *read_onlogn_voids(char *vpath, int *vnumber)
{
	Tuple *a;
	FILE *vfp;
	int tmp_int0;
	int tmp_int1;
	float tmp_flt0;
	int i;
	
	vfp = fopen(vpath, "r");
	
	if(vfp == NULL)
	{
		printf("** ERROR ** read_onlogn_voids: No se pudo abrir '%s'.\n", vpath);
		exit(EXIT_FAILURE);
	}
	
	/* Leer número de triángulos. */
	fscanf(vfp, "%d\n", vnumber);
	
	a = (Tuple *)malloc((*vnumber)*sizeof(Tuple));
	
	for(i = 0; i < *vnumber; i++)
	{
		/* Ignorar los tmp_flt0 */
		fscanf(vfp, "%d \t %d \t %f \t %f \t %f \t %f \t %f \t %f \n", &tmp_int0, &tmp_int1, &tmp_flt0,
						&tmp_flt0, &tmp_flt0, &tmp_flt0, &tmp_flt0, &tmp_flt0);
		
		/* Inicializar campos. */
		a[i] = (Tuple){.r = tmp_int0, .t = tmp_int1};
	}
	
	fclose(vfp);
	
	return a;
}



/* read_on_voids
 * 
 * Lee un archivo _iv.dat de la implementación O(n).
 * */

Tuple *read_on_voids(char *vpath, int *vnumber)
{
	Tuple *a;
	FILE *vfp;
	int tmp_int0;
	int tmp_int1;
	double tmp_dbl0;
	int i;
	
	vfp = fopen(vpath, "r");
	
	if(vfp == NULL)
	{
		printf("** ERROR ** read_on_voids: No se pudo abrir '%s'.\n", vpath);
		exit(EXIT_FAILURE);
	}
	
	/* Leer número de triángulos. */
	fscanf(vfp, "%d\n", vnumber);
	
	a = (Tuple *)malloc((*vnumber)*sizeof(Tuple));
	
	for(i = 0; i < *vnumber; i++)
	{
		fscanf(vfp, "%d %d %lf\n", &tmp_int0, &tmp_int1, &tmp_dbl0);
		
		/* Inicializar campos. */
		a[i] = (Tuple){.r = tmp_int0, .t = tmp_int1};
	}
	
	fclose(vfp);
	
	return a;
}



/* block_qsort
 * 
 * Sobre un arreglo de tuplas {r,t}, hace quicksort sobre
 * cada bloque de componentes r iguales.
 * */

static void block_qsort(Tuple *a, int vnumber)
{
	int i;
	int current_r;
	int block_size;
	int start_index;
	
	/* En cada bloque de identificadores de vacío iguales, ordenar
	 * por identificadores de triángulos.*/
	current_r = a[0].r;
	start_index = 0;
	block_size = 0;
	
	for(i = 0; i < vnumber; i++)
	{
		if((current_r == a[i].r) && (i == vnumber - 1))
		{
			block_size = block_size + 1;
			qsort(a + start_index, block_size, sizeof(Tuple), compare_tuple_t);
		}
		else if((current_r == a[i].r) && (i < vnumber - 1))
		{
			block_size = block_size + 1;
		}
		else if((current_r != a[i].r) && (i == vnumber - 1))
		{
			/* Nada. El último vacío contiene un solo triángulo.
			 * No tiene sentido ordenar. */
		}
		else if((current_r != a[i].r) && (i < vnumber - 1))
		{
			qsort(a + start_index, block_size, sizeof(Tuple), compare_tuple_t);
			start_index = i;
			block_size = 1;
			current_r = a[i].r;
		}
	}

}


/* normalize_voids
 * 
 * Transforma un arreglo de tuplas {r,t}, de modo que:
 * - El identificador de cada vacío sea el mínimo de los identificadores de sus
 *   triángulos asociados. Serán llamados identificadores normalizados de vacíos.
 * - Esté ordenado por identificador normalizado de vacío.
 * - Cada bloque de identificador normalizado de vacío, esté ordenado por
 *   identificador de triángulo.
 * */

static void normalize_voids(Tuple *a, int vnumber)
{
	int i;
	int current_r;
	int current_t;
	
	/* Ordenar por identificador de vacío. */
	qsort(a, vnumber, sizeof(Tuple), compare_tuple_r);
	
	/* En cada bloque de identificadores de vacío iguales, ordenar
	 * por identificadores de triángulos.*/
	block_qsort(a, vnumber);
	
	/* En cada inicio de bloque, copiar el primer identificador
	 * del triángulo al identificador de vacío. */
	current_r = a[0].r;
	current_t = a[0].t;
	
	for(i = 0; i < vnumber; i++)
	{
		if(current_r != a[i].r)
		{
			current_r = a[i].r;
			current_t = a[i].t;
		}
		
		a[i].r = current_t;
	}
	
	/* Ordenar por identificador de vacío. */
	qsort(a, vnumber, sizeof(Tuple), compare_tuple_r);
	
	/* En cada bloque de identificadores de vacío iguales, ordenar
	 * por identificadores de triángulos.*/
	block_qsort(a, vnumber);
}



/* compare_voids
 * 
 * Indica si el arreglo de vacíos a es igual al b.
 * */

int compare_voids(Tuple *a, Tuple *b, int vnumber)
{
	int i;
	
	normalize_voids(a, vnumber);
	
	normalize_voids(b, vnumber);
	
	for(i = 0; i < vnumber; i++)
	{
		if((a[i].r != b[i].r) || (a[i].t != b[i].t))
		{
			/* Se detectó diferencia. */
			return FALSE;
		}
	}
	
	/* No se detectaron diferencias. Son iguales. */
	return TRUE;
}
