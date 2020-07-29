/* Programa principal para gráficos SVG de triangulaciones. */

#include <stdlib.h>
#include <stdio.h>
#include "../shared/io.h"
#include "../shared/consts.h"
#include "svgio.h"
#include "args.h"
#include "aux.h"
#include "../csec/triang.h"



int main(int argc, char **argv)
{
	int pnumber;
	int tnumber;
	int inumber;
	int i;
	int j;
	int labels;
	int points;
	int circles;
	
	double *r;
	int *p;
	int *adj;
	int *visited;
	int *max;
	int *id;
	
	char *ppath;
	char *cpath;
	char *spath;
	
	read_arguments(argc, argv, &ppath, &cpath, &spath, &labels, &points, &circles);
	
	read_qdelaunay_data(ppath, &r, &p, &adj, NULL, &pnumber, &tnumber, NULL);
	
	read_triangles(cpath, &id, &inumber);
	
	FILE *sp;
	
	sp = fopen(spath, "w+");
	
	if(sp == NULL)
	{
		printf("** ERROR ** No se pudo abrir '%s'.\n", spath);
		exit(EXIT_FAILURE);
	}
	
	double xmin;
	double ymin;
	double xmax;
	double ymax;
	
	extremal_coords(r, pnumber, &xmin, &ymin, &xmax, &ymax);
	write_svg_header(sp, xmin, ymin, xmax, ymax);
	
	/* Crear arreglos. */
	max = (int *)malloc(tnumber*sizeof(int));
	visited = (int *)malloc(tnumber*sizeof(int));
	
	/* Inicializar arreglos. */
	for(i = 0; i < tnumber; i++)
	{
		visited[i] = FALSE;
		max[i] = max_edge_index(i, r, p);
	}
	
	
	/* Escribir circunferencias circunscritas. */
	if(circles)
	{
		write_svg_comment(sp, "Circunferencias circunscritas");
		for(i = 0; i < tnumber; i++)
		{
			double x0;
			double y0;
			double x1;
			double y1;
			double x2;
			double y2;
			double x;
			double y;
			double rad;
			
			x0 = r[2*p[3*i + 0] + 0];
			y0 = r[2*p[3*i + 0] + 1];
			x1 = r[2*p[3*i + 1] + 0];
			y1 = r[2*p[3*i + 1] + 1];
			x2 = r[2*p[3*i + 2] + 0];
			y2 = r[2*p[3*i + 2] + 1];
			
			triangle_circumcircle(x0, y0, x1, y1, x2, y2, &x, &y, &rad);
			
			write_svg_circumcircle(sp, x, y ,rad);
		}
	}
	
	/* Escribir triángulos. */
	write_svg_comment(sp, "Triángulos");
	for(i = 0; i < inumber; i++)
	{
		double x0;
		double y0;
		double x1;
		double y1;
		double x2;
		double y2;
		
		j = id[i];
		
		x0 = r[2*p[3*j + 0] + 0];
		y0 = r[2*p[3*j + 0] + 1];
		x1 = r[2*p[3*j + 1] + 0];
		y1 = r[2*p[3*j + 1] + 1];
		x2 = r[2*p[3*j + 2] + 0];
		y2 = r[2*p[3*j + 2] + 1];
		
		write_svg_triangle(sp, x0, y0, x1, y1, x2, y2);
	}
	
	/* Escribir aristas e identificadores de triángulos. */
	write_svg_comment(sp, "Aristas y etiquetas de triángulos");
	svg_adj_graph_DFS(0, sp, r, p, adj, visited, max, labels);
	
	/* Escribir puntos. */
	if(points)
	{
		write_svg_comment(sp, "Puntos y sus etiquetas");
		char tmp_str[128];
		
		for(i = 0; i < pnumber; i++)
		{
			double x;
			double y;
			
			x = r[2*i + 0];
			y = r[2*i + 1];
			
			if(labels)
			{
				sprintf(tmp_str, "%d", i);
				write_svg_point(sp, x, y, tmp_str);
			}
			else
			{
				write_svg_point(sp, x, y, "");
			}
		}
	}
	
	/* Terminar SVG y cerrar archivo. */
	write_svg_end(sp);
	
	fclose(sp);
	
	free(r);
	free(p);
	free(adj);
	free(visited);
	free(max);
	free(id);
	
	return EXIT_SUCCESS;
}
