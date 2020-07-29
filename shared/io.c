/* Funciones para manejar entrada y salida de datos. */

#define _POSIX_C_SOURCE 200112L /* Para popen, pclose y posix_memalign. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "consts.h"

/*#define cmdpath "/home/jojeda/qhull-2015.2/bin/qdelaunay"*/
/* #define cmdpath ""
#define cmdname "qdelaunay" */
#define cmdpath "/home/ansep/jojeda/qhull-2015.2/bin/qdelaunay"
#define cmdname "qdelaunay"

#define NZ_FILE_SUFFIX "_nz.dat"
#define IV_FILE_SUFFIX "_iv.dat"
#define BV_FILE_SUFFIX "_bv.dat"
#define W_FILE_SUFFIX "_w.dat"

/* read_qdelaunay_data
 * 
 * Introduce el archivo de puntos ppath a qdelaunay y luego introduce
 * los datos que genera a r, p, adj, a. A pnumber y tnumber le introduce
 * el número de puntos y triángulos, correspondientemente.
 * 
 * El puntero a áreas puede ser NULL.
 * 
 * También recibe align_settings que es un arreglo de 2 componentes:
 * un puntero a entero que indica alineación y un puntero a función que
 * toma el tamaño de la memoria a reservar y retorna un entero.
 * */

void read_qdelaunay_data(char *ppath, double **r, int **p, int **adj,
													double **a, int *pnumber, int *tnumber, void **align_settings)
{
	FILE *qdp;
	
	char cmd[1024];
	int alignment;
	int (*new_aligned_mem_size)(int);
	
	if(align_settings != NULL)
	{
		/* Obtener alineamiento y función para calcular el tamaño
		 * de la memoria alineada. */
		alignment = *((int *)(align_settings[0]));
		new_aligned_mem_size = align_settings[1];
	}
	
	/* Construir comando a ejecutar. */
	strcpy(cmd, cmdpath);
	strcat(cmd, " i Fn Fa QJ < "); /* QJ para que no hayan triángulos iguales!! */
	strcat(cmd, ppath);
	qdp = popen(cmd, "r");
	
	if(qdp == NULL)
	{
		printf("** ERROR ** read_qdelaunay_data: No se pudo ejecutar qdelaunay.\n" );
		exit(EXIT_FAILURE);
	}
	
	int i;
	int tmp_int0;
	int tmp_int1;
	int tmp_int2;
	double tmp_dbl0;
	double tmp_dbl1;
	
	/* Leer datos de hijo, a través de tubería.
	 * Leer número de triángulos. */
	fscanf(qdp, "%d\n", tnumber);
	
	if(align_settings == NULL)
	{
		*p = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)p, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	
	/* Leer puntos de cada triángulo y verterlos en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "%d %d %d\n", &tmp_int0, &tmp_int1, &tmp_int2);
		
		/* Inicializar campos. */
		(*p)[3*i + 0] = tmp_int0;
		(*p)[3*i + 1] = tmp_int1;
		(*p)[3*i + 2] = tmp_int2;
	}
  
	/* Ignorar línea */
	fscanf(qdp, "%*[^\n]\n");
	
	if(align_settings == NULL)
	{
		*adj = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)adj, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	
	/* Leer identificadores de triaǵnulos adyacentes a
	 * cada triángulo y verterlos en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "3 %d %d %d\n", &tmp_int0, &tmp_int1, &tmp_int2);
		
		/* Inicializar campos. Si no hay adyacencia, colocar TRIANG_BORDER. */
		(*adj)[3*i + 0] = (tmp_int0 < 0) ? TRIANG_BORDER : tmp_int0;
		(*adj)[3*i + 1] = (tmp_int1 < 0) ? TRIANG_BORDER : tmp_int1;
		(*adj)[3*i + 2] = (tmp_int2 < 0) ? TRIANG_BORDER : tmp_int2;
	}
	
	/* Ignorar línea */
	fscanf(qdp, "%*[^\n]\n");
	
	if(a != NULL)
	{
		if(align_settings == NULL)
		{
			*a = (double *)malloc((*tnumber)*sizeof(double));
		}
		else
		{
			posix_memalign((void **)a, alignment, new_aligned_mem_size((*tnumber)*sizeof(double)));
		}
	}
	
	/* Leer áreas de cada triángulo y verterlas en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "%lf\n", &tmp_dbl0);
		
		if(a != NULL)
		{
			/* Inicializar campos. */
			(*a)[i] = tmp_dbl0;
		}
	}
	
	/* Pudiera quedar algo por leer, pero no interesa. */
	while(fscanf(qdp, "%*[^\n]\n") > 0)
	{
		/* Descartar. */
	}
	
	pclose(qdp);
	
	FILE *pf = fopen(ppath, "r");
	
	/* Ignorar primera línea. */
	fscanf(pf, "%*[^\n]\n");
	
	/* Leer número de puntos. */
	fscanf(pf, "%d", pnumber);
	
	if(align_settings == NULL)
	{
		*r = (double *)malloc(2*(*tnumber)*sizeof(double));
	}
	else
	{
		posix_memalign((void **)r, alignment, new_aligned_mem_size(2*(*tnumber)*sizeof(double)));
	}
	
	for(i = 0; i < *pnumber; i++)
	{
		fscanf(pf, "%lf %lf", &tmp_dbl0, &tmp_dbl1);
		
		/* Inicializar campos. */
		(*r)[2*i + 0] = tmp_dbl0;
		(*r)[2*i + 1] = tmp_dbl1;
	}
	
	fclose(pf);
}



/* write_classification
 * 
 * Escribe la clasificación de los triángulos en archivos con prefijo cpath_prefix.
 * */

void write_classification(char *cpath_prefix, int *root_id, int *type, double *area,
													int tnumber, int num_nonzone_triangs,
													int num_ivoid_triangs, int num_bvoid_triangs, int num_wall_triangs)
{
	FILE *fp_nz;
	FILE *fp_iv;
	FILE *fp_bv;
	FILE *fp_w;
	FILE *fp_tmp;
	int i;
	
	int cpath_prefix_length;
	
	cpath_prefix_length = strlen(cpath_prefix);
	
	char nz_path[cpath_prefix_length + strlen(NZ_FILE_SUFFIX)];
	char iv_path[cpath_prefix_length + strlen(IV_FILE_SUFFIX)];
	char bv_path[cpath_prefix_length + strlen(BV_FILE_SUFFIX)];
	char w_path[cpath_prefix_length + strlen(W_FILE_SUFFIX)];
	
	/* Construir nombres de archivo. */
	strcpy(nz_path, cpath_prefix);
	strcpy(iv_path, cpath_prefix);
	strcpy(bv_path, cpath_prefix);
	strcpy(w_path, cpath_prefix);
	
	strcat(nz_path, NZ_FILE_SUFFIX);
	strcat(iv_path, IV_FILE_SUFFIX);
	strcat(bv_path, BV_FILE_SUFFIX);
	strcat(w_path, W_FILE_SUFFIX);
	
	/* Abrir archivos. */
	fp_nz = fopen(nz_path, "w+");
	fp_iv = fopen(iv_path, "w+");
	fp_bv = fopen(bv_path, "w+");
	fp_w = fopen(w_path, "w+");
	
	/* Escribir número de triángulos. */
	fprintf(fp_nz, "%d\n", num_nonzone_triangs);
	fprintf(fp_iv, "%d\n", num_ivoid_triangs);
	fprintf(fp_bv, "%d\n", num_bvoid_triangs);
	fprintf(fp_w, "%d\n", num_wall_triangs);
	
	/* Escribir datos. */
	for(i = 0; i < tnumber; i++)
	{
		/* Dependiendo del tipo, se escribirá en uno y otro archivo. */
		if(type[i] == NONZONE)
		{
			fp_tmp = fp_nz;
		}
		else if(type[i] == INNER_VOID)
		{
			fp_tmp = fp_iv;
		}
		else if(type[i] == BORDER_VOID)
		{
			fp_tmp = fp_bv;
		}
		else if(type[i] == WALL)
		{
			fp_tmp = fp_w;
		}
		else
		{
			printf("** ERROR ** write_classification: Índice de clasificación de triángulo desconocido.\n" );
			exit(EXIT_FAILURE);
		}
		fprintf(fp_tmp, "%d %d %lf\n", root_id[i], i, area[i]);
	}
	
	/* Cerrar archivos. */
	fclose(fp_nz);
	fclose(fp_iv);
	fclose(fp_bv);
	fclose(fp_w);
}



/* read_triangles
 * 
 * Lee los identificadores de los triángulos contenidos en el archivo cpath.
 * */

void read_triangles(char *cpath, int **id, int *inumber)
{
	FILE *pf;
	
	int tmp_int0;
	int tmp_int1;
	double tmp_dbl0;
	int i;
	
	pf = fopen(cpath, "r");
	
	if(pf == NULL)
	{
		printf("** ERROR ** read_classification: No se pudo abrir '%s'.\n", cpath);
		exit(EXIT_FAILURE);
	}
	
	/* Leer número de regiones (zonas + no-zonas). */
	fscanf(pf, "%d", inumber);
	
	*id = (int *)malloc((*inumber)*sizeof(int));
	
	for(i = 0; i < *inumber; i++)
	{
		fscanf(pf, "%d %d %lf", &tmp_int0, &tmp_int1, &tmp_dbl0);
		
		/* Inicializar campos. */
		(*id)[i] = tmp_int1;
	}
	
	fclose(pf);
}
