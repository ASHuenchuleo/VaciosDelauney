/* Programa principal para O(n) secuencial. */

#include <stdlib.h>
#include <stdio.h>
#include "../shared/io.h"
#include "../shared/args.h"
#include "../shared/consts.h"
#include "../shared/adjgraph.h"
#include "../shared/timestamp.h"
#include "triang.h"



int main(int argc, char **argv)
{
	int pnumber;
	int tnumber;
	double *r;
	int *p;
	int *adj;
	double *area;
	char *ppath;
	double threshold;
	char *cpath_prefix;
	struct timeval t;
	double elapsed;
	
	FILE *sfd;
	sfd = fopen("dat/stats.dat", "a");
	start_time_measure(&t);
	
	read_arguments(argc, argv, &ppath, &threshold, &cpath_prefix);
	
	print_timestamp("Ejecutando qdelaunay...\n", t);
	read_qdelaunay_data(ppath, &r, &p, &adj, &area, &pnumber, &tnumber, NULL);

	
	printf("* %d triángulos\n", tnumber);
	printf("* %d puntos\n", pnumber);
	elapsed = print_timestamp("Procesando...\n", t);
	fprintf(sfd, "csec %d %d %lf ", pnumber, tnumber, elapsed);
	
	int *max;
	int *is_seed;
	int *type;
	int *visited;
	int *root_id;
	
	max = (int *)malloc(tnumber*sizeof(int));
	is_seed = (int *)malloc(tnumber*sizeof(int));
	type = (int *)malloc(tnumber*sizeof(int));
	visited = (int *)malloc(tnumber*sizeof(int));
	root_id = (int *)malloc(tnumber*sizeof(int));
	
	int i;
	int j;
	
	/* Inicializar arreglos adicionales. */
	for(i = 0; i < tnumber; i++)
	{
		is_seed[i] = FALSE;
		visited[i] = FALSE;
	}
	
	/* Etapa 1: Encontrar aristas máximas. */
	for(i = 0; i < tnumber; i++)
	{
		max[i] = max_edge_index(i, r, p);
	}
	
	/* Etapa 2: Desconectar arcos asociados a aristas nomáx-nomáx. */
	for(i = 0; i < tnumber; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if((adj[3*i + j] != TRIANG_BORDER) && is_nomax_nomax(i, adj[3*i + j], p, max))
			{
				adj[3*i + j] = NO_ADJ;
			}
		}
	}
	
	/* Etapa 3: Marcar triángulos semilla. */
	for(i = 0; i < tnumber; i++)
	{
		for(j = 0; j < 3; j++)
		{
			int k;
			
			k = adj[3*i + j];
			
			if((k != NO_ADJ) && (k != TRIANG_BORDER) && is_max_max(i, adj[3*i + j], p, max) && (k < i))
			{
				is_seed[i] = TRUE;
			}
		}
	}
	
	/* Etapa 4: Hacer un DFS a cada nodo no visitado, para comunicarle
	 * el área total de los triángulos que subtiende, si hay alguna semilla
	 * entre ellos y si alguno toca el borde de la triangulación. */
	int num_regs;
	int num_nonzones;
	int num_ivoids;
	int num_bvoids;
	int num_walls;
	int num_nonzone_triangs;
	int num_ivoid_triangs;
	int num_bvoid_triangs;
	int num_wall_triangs;
	
	num_regs = 0;
	num_nonzones = 0;
	num_ivoids = 0;
	num_bvoids = 0;
	num_walls = 0;
	num_nonzone_triangs = 0;
	num_ivoid_triangs = 0;
	num_bvoid_triangs = 0;
	num_wall_triangs = 0;
	
	for(i = 0; i < tnumber; i++)
	{
		if(!visited[i])
		{
			/* Se tiene una nueva raíz. */
			double total_area;
			int has_seed;
			int touches_border;
			int total_triangles;
			
			has_seed = FALSE;
			total_area = 0.0;
			touches_border = FALSE;
			total_triangles = 0;
			
			/* Hacer DFS */
			adj_graph_DFS(i, i, adj, visited, area, root_id, is_seed, &total_area,
								&has_seed, &touches_border, &total_triangles);
			
			area[i] = total_area;
			
			/* Clasificar raíz */
			if(has_seed && total_area >= threshold && !touches_border)
			{
				type[i] = INNER_VOID;
				num_ivoids++;
				num_ivoid_triangs = num_ivoid_triangs + total_triangles;
			}
			else if(has_seed && total_area >= threshold && touches_border)
			{
				type[i] = BORDER_VOID;
				num_bvoids++;
				num_bvoid_triangs = num_bvoid_triangs + total_triangles;
			}
			else if(has_seed && total_area < threshold)
			{
				type[i] = WALL;
				num_walls++;
				num_wall_triangs = num_wall_triangs + total_triangles;
			}
			else
			{
				type[i] = NONZONE;
				num_nonzones++;
				num_nonzone_triangs = num_nonzone_triangs + total_triangles;
			}
			
			num_regs++;
		}
	}
	
	/* Etapa 5: Propagar a cada triángulo el área total de la región
	 * (zona o no-zona) a la que pertenece y el tipo de ella. */
	for(i = 0; i < tnumber; i++)
	{
		area[i] = area[root_id[i]];
		type[i] = type[root_id[i]];
	}
	
	printf("* Número de vacíos internos: %d (%d triángulos)\n", num_ivoids, num_ivoid_triangs);
	printf("* Número de vacíos de borde: %d (%d triángulos)\n", num_bvoids, num_bvoid_triangs);
	printf("* Número de murallas: %d (%d triángulos)\n", num_walls, num_wall_triangs);
	printf("* Número de no-zonas: %d (%d triángulos)\n", num_nonzones, num_nonzone_triangs);
	
	elapsed = print_timestamp("Escribiendo datos...\n", t);
	fprintf(sfd, "%lf ", elapsed);
	
	write_classification(cpath_prefix, root_id, type, area, tnumber, num_nonzone_triangs,
												num_ivoid_triangs, num_bvoid_triangs, num_wall_triangs);
	
	free(r);
	free(p);
	free(adj);
	free(area);
	
	free(max);
	free(is_seed);
	free(type);
	free(visited);
	free(root_id);
	
	elapsed = print_timestamp("Fin.\n", t);
	fprintf(sfd, "%lf\n", elapsed);
	
	fclose(sfd);
	return EXIT_SUCCESS;
}
