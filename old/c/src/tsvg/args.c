/* Funciones para manejar argumentos de línea de comando. */

#include <stdlib.h>
#include <stdio.h>
#include "../shared/consts.h"

void read_arguments(int argc, char **argv, char **ppath, char **vpath,
											char **spath, int *labels, int *points, int *circles)
{
	/* Chequear si hay suficientes argumentos. */
	if(argc < 7)
	{
		printf("Uso: %s [archivo puntos] [archivo clasificación] ", argv[0]);
		printf("[archivo gráfico svg] [etiquetas?] [puntos?] [círculos?]\n");
		printf("Parámetros [etiquetas?], [puntos?] y [círculos?] deben ser 0 (no) o distinto de 0 (sí).\n");
		exit(EXIT_FAILURE);
	}
	
	/* Obtener argumentos de línea de comando. */
	*ppath = argv[1];
	*vpath = argv[2];
	*spath = argv[3];
	*labels = atoi(argv[4]);
	*points = atoi(argv[5]);
	*circles = atoi(argv[6]);
}
