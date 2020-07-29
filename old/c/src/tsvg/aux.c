/* Funciones auxiliares para imprimir triangulación como SVG. */

#include <stdio.h>
#include <math.h>
#include "../csec/triang.h"
#include "../shared/consts.h"
#include "svgio.h"
#include "consts.h"



/* triangle_circumcircle
 * 
 * Calcula el circuncentro y radio de la circunferencia circunscrita al
 * triángulo {(x0,y0),(x1,y1),(x2,y2)} y lo deja en (x,y) y r.
 * */

void triangle_circumcircle(double x0, double y0, double x1, double y1,
											double x2, double y2, double *x, double *y, double *r)
{
	double d;
	double l0;
	double l1;
	double l2;
	double tmp;
	
	/* https://en.wikipedia.org/wiki/Circumscribed_circle */
	d = 2*(x0*(y1 - y2) + x1*(y2 - y0) + x2*(y0 - y1));
	
	*x = ((x0*x0 + y0*y0)*(y1 - y2) + (x1*x1 + y1*y1)*(y2 - y0) + (x2*x2 + y2*y2)*(y0 - y1))/d;
	*y = ((x0*x0 + y0*y0)*(x2 - x1) + (x1*x1 + y1*y1)*(x0 -x2) + (x2*x2 + y2*y2)*(x1 - x0))/d;
	
	l0 = dist(x0, y0, *x, *y);
	l1 = dist(x1, y1, *x, *y);
	l2 = dist(x2, y2, *x, *y);
	
	tmp = l0 < l1 ? l1 : l0;
	*r = tmp < l2 ? l2 : tmp; 
}



/* triangle_incenter
 * 
 * Calcula el incentro del triángulo {(x0,y0),(x1,y1),(x2,y2)}
 * y lo deja en (x,y).
 * */

static void triangle_incenter(double x0, double y0, double x1, double y1,
											double x2, double y2, double *x, double *y)
{
	double l0;
	double l1;
	double l2;
	
	l0 = dist(x0, y0, x1, y1);
	l1 = dist(x1, y1, x2, y2);
	l2 = dist(x2, y2, x0, y0);
	
	*x = (x0*l1 + x1*l2 + x2*l0)/(l0 + l1 + l2);
	*y = (y0*l1 + y1*l2 + y2*l0)/(l0 + l1 + l2);
}



/* extremal_coords
 * 
 * Obtiene las coordenadas (xmax,ymax) y (xmin,ymin) extremas del arreglo
 * de coordenadas de puntos r.
 * */

void extremal_coords(double *r, int pnumber, double *xmin, double *ymin,
											double *xmax, double *ymax)
{
	double x;
	double y;
	int i;
	
	for(i = 0; i < pnumber; i++)
	{
		x = r[2*i + 0];
		y = r[2*i + 1];
		
		*xmin = *xmin < x ? *xmin : x;
		*ymin = *ymin < y ? *ymin : y;
		*xmax = *xmax > x ? *xmax : x;
		*ymax = *ymax > y ? *ymax : y;
	}
}



/* svg_adj_graph_DFS
 * 
 * Realiza un DFS en el grafo de adyacencia, para imprimir en SVG
 * aristas y etiquetas de triángulos. 
 * */

void svg_adj_graph_DFS(int i, FILE *sp, double *r, int *p, int *adj,
												int *visited, int *max, int labels)
{
	int j;
	int k;
	int l;
	double x0;
	double y0;
	double x1;
	double y1;
	double x2;
	double y2;
	double x;
	double y;
	
	/* Imprimir etiqueta de triángulo i. */
	char tmp_str[128];
	x0 = r[2*p[3*i + 0] + 0];
	y0 = r[2*p[3*i + 0] + 1];
	x1 = r[2*p[3*i + 1] + 0];
	y1 = r[2*p[3*i + 1] + 1];
	x2 = r[2*p[3*i + 2] + 0];
	y2 = r[2*p[3*i + 2] + 1];
	triangle_incenter(x0, y0, x1, y1, x2, y2, &x, &y);
	
	if(labels)
	{
		sprintf(tmp_str, "%d", i);
		write_svg_label(sp, x, y, tmp_str);
	}
	
	
	/* Imprimir las 3 aristas coloreadas de acuerdo a su tipo. */
	for(j = 0; j < 3; j++)
	{
		int adj_id;
		
		k = p[3*i + j];
		l = p[3*i + (j + 1)%3];
		
		adj_id = get_adjacent_triangle(i, k, l, p, adj);
		
		if((adj_id != NO_ADJ) && visited[adj_id])
		{
			/* Si un triángulo ya fue visitado, es porque ya todas sus aristas
			 * fueron dibujadas. Entonces no dibujar la propuesta en esta iteración. */
			continue;
		}
		
		x0 = r[2*k + 0];
		y0 = r[2*k + 1];
		x1 = r[2*l + 0];
		y1 = r[2*l + 1];
		
		if(adj_id == NO_ADJ)
		{
			write_svg_edge(sp, x0, y0, x1, y1, BORDER_EDGE);
		}
		else
		{
			if(is_max_max(i, adj_id, p, max))
			{
				write_svg_edge(sp, x0, y0, x1, y1, MAXMAX_EDGE);
			}
			else if(is_nomax_nomax(i, adj_id, p, max))
			{
				write_svg_edge(sp, x0, y0, x1, y1, NOMAXNOMAX_EDGE);
			}
			else
			{
				write_svg_edge(sp, x0, y0, x1, y1, MAXNOMAX_EDGE);
			}
		}
	}
	
	/* Marcar visita. */
	visited[i] = TRUE;
	
	/* Avanzar. */
	for(j = 0; j < 3; j++)
	{
		k = adj[3*i + j];
		/* Aquí no se ha hecho partición del grafo, por lo que
		 * no hay adyacencias del tipo NO_ADY; sólo a triángulos y a
		 * borde de triangulación. */
		if((k != TRIANG_BORDER) && !visited[k])
		{
			svg_adj_graph_DFS(k, sp, r, p, adj, visited, max, labels);
		}
	}
}
