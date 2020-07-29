/* Funciones para manejar triangulación. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../shared/consts.h"



/* dist
 * 
 * Retorna la distancia que hay entre el punto (x0,y0)
 * y (x1,y1).
 * */

double dist(double x0, double y0, double x1, double y1)
{
	return sqrt(pow(x0 - x1, 2.0) + pow(y0 - y1, 2.0));
}



/* max_edge_index
 * 
 * Retorna el índice k de la arista máxima de un triángulo i, 
 * descrito por los puntos p0p1p2. Será 0 si p0p1 es máxima.
 * Será 1 si p1p2 lo es. Será 2 si p2p0 lo es.
 * */

int max_edge_index(int i, double *r, int *p)
{
	double l0;
	double l1;
	double l2;
	
	int p0;
	int p1;
	int p2;
	
	p0 = p[3*i + 0];
	p1 = p[3*i + 1];
	p2 = p[3*i + 2];
	
	l0 = dist(r[2*p0 + 0], r[2*p0 + 1], r[2*p1 + 0], r[2*p1 + 1]);
	l1 = dist(r[2*p1 + 0], r[2*p1 + 1], r[2*p2 + 0], r[2*p2 + 1]);
	l2 = dist(r[2*p2 + 0], r[2*p2 + 1], r[2*p0 + 0], r[2*p0 + 1]);
	
	if((l0 >= l1 && l1 >= l2) || (l0 >= l2 && l2 >= l1))
	{
		return 0;
	}
	else if((l1 >= l0 && l0 >= l2) || (l1 >= l2 && l2 >= l0))
	{
		return 1;
	}
	else
	{
		return 2;
	}
}



/* same_edge
 * 
 * Indica para las aristas {u,v} y {w,x} si son iguales o no.
 * */
 
static int same_edge(int u, int v, int w, int x)
{
	return (u == w && v == x) || (u == x && v == w);
}



/* get_edge_index
 * 
 * Entrega el índice de la arista {u,v} respecto del triángulo i.
 * */

static int get_edge_index(int u, int v, int i, int *p)
{
	int p0;
	int p1;
	int p2;
	
	p0 = p[3*i + 0];
	p1 = p[3*i + 1];
	p2 = p[3*i + 2];
	
	if(same_edge(u, v, p0, p1))
	{
		return 0;
	}
	else if(same_edge(u, v, p1, p2))
	{
		return 1;
	}
	else if(same_edge(u, v, p2, p0))
	{
		return 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** get_edge_index: Arista {%d,%d} no pertenece al triángulo %d.\n", u, v, i);
		exit(EXIT_FAILURE);
	}
}



/* is_nomax_nomax
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es nomáx-nomáx.
 * */

int is_nomax_nomax(int i, int j, int *p, int *max)
{
	int p0i;
	int p1i;
	int p2i;
	int p0j;
	int p1j;
	int p2j;
	
	p0i = p[3*i + 0];
	p1i = p[3*i + 1];
	p2i = p[3*i + 2];
	
	p0j = p[3*j + 0];
	p1j = p[3*j + 1];
	p2j = p[3*j + 2];
	
	int ij;
	int ii;
	
	if(same_edge(p0i, p1i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** is_nomax_nomax: Problema insperado para triángulos %d y %d.\n", i, j);
		exit(EXIT_FAILURE);
	}
	
	return (ij != max[j]) && (ii != max[i]);
}



/* is_max_max
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es máx-máx.
 * */

int is_max_max(int i, int j, int *p, int *max)
{
	int p0i;
	int p1i;
	int p2i;
	
	int p0j;
	int p1j;
	int p2j;
	
	p0i = p[3*i + 0];
	p1i = p[3*i + 1];
	p2i = p[3*i + 2];
	
	p0j = p[3*j + 0];
	p1j = p[3*j + 1];
	p2j = p[3*j + 2];
	
	int ij;
	int ii;
	
	if(same_edge(p0i, p1i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** is_max_max: Problema insperado para triángulos %d y %d.\n", i, j);
		exit(EXIT_FAILURE);
	}
	
	return (ij == max[j]) && (ii == max[i]);
}



/* edge_belongs_to
 * 
 * Indica si arista {k,l} pertenece al triángulo i.
 * */

static int edge_belongs_to(int k, int l, int i, int *p)
{
	return same_edge(k, l, p[3*i + 0], p[3*i + 1])
					|| same_edge(k, l, p[3*i + 1], p[3*i + 2])
					|| same_edge(k, l, p[3*i + 2], p[3*i + 0]);
}



/* get_adjacent_triangle
 * 
 * Retorna el identificador del triángulo que es adyacente al
 * triángulo i, mediante la arista {k,l}.
 * 
 * Si no hay triángulo, retorna NO_ADY (aún si es porque {k,l}
 * es de borde de triangulación).
 * */

int get_adjacent_triangle(int i, int k, int l, int *p, int *adj)
{
	int u;
	int v;
	int w;
	
	/* Comprobar que {k,l} pertenezca al triángulo i. */
	if(!edge_belongs_to(k, l, i, p))
	{
		fprintf(stderr, "** ERROR ** get_adjacent_triangle: Arista {%d,%d} no pertenece al triángulo %d.\n", k, l, i);
		exit(EXIT_FAILURE);
	}
	
	u = adj[3*i + 0];
	v = adj[3*i + 1];
	w = adj[3*i + 2];
	
	int index;
	
	if((u != NO_ADJ) && (u != TRIANG_BORDER) && (same_edge(k, l, p[3*u + 0], p[3*u + 1]) ||
			same_edge(k, l, p[3*u + 1], p[3*u + 2]) || same_edge(k, l, p[3*u + 2], p[3*u + 0])))
	{
		index = u;
	}
	else if((v != NO_ADJ) && (v != TRIANG_BORDER) && (same_edge(k, l, p[3*v + 0], p[3*v + 1]) ||
					same_edge(k, l, p[3*v + 1], p[3*v + 2]) || same_edge(k, l, p[3*v + 2], p[3*v + 0])))
	{
		index = v;
	}
	else if((w != NO_ADJ) && (w != TRIANG_BORDER) && (same_edge(k, l, p[3*w + 0], p[3*w + 1]) ||
					same_edge(k, l, p[3*w + 1], p[3*w + 2]) || same_edge(k, l, p[3*w + 2], p[3*w + 0])))
	{
		index = w;
	}
	else
	{
		/* Ningún triángulo apareció como adyacente a {k,l} desde el i. */
		index = NO_ADJ;
	}
	
	return index;
}
