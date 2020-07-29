/* Funciones para manejar argumentos de línea de comando. */

#include <stdlib.h>
#include <stdio.h>


/* read_arguments
 * 
 * Lee los argumentos desde el arreglo de cadenas argv.
 * */

void read_arguments(int argc, char **argv, char **ppath, double *threshold,
											char **cpath_prefix)
{
	/* Chequear si hay suficientes argumentos. */
	if(argc < 4)
	{
		printf("Uso: %s [archivo puntos] ", argv[0]);
		printf("[threshold] [prefijo archivos clasificación]\n");
		exit(EXIT_FAILURE);
	}
	
	/* Obtener argumentos de línea de comando. */
	*ppath = argv[1];
	*threshold = atof(argv[2]);
	*cpath_prefix = argv[3];
}
