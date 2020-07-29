/* Funciones para lectura de argumentos de línea de comando. */

#include <stdlib.h>
#include <stdio.h>



void read_arguments(int argc, char **argv, char **onlogn_path, char **on_path)
{
	/* Chequear si hay suficientes argumentos. */
	if(argc < 3)
	{
		printf("Uso: %s [vacíos O(n*log n)] [vacíos O(n)]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* Obtener argumentos de línea de comando. */
	*onlogn_path = argv[1];
	*on_path = argv[2];
}
