

/* same_edge
 * 
 * Indica para las aristas {u,v} y {w,x} si son iguales o no.
 * */
 
int same_edge(int u, int v, int w, int x)
{
	return (u == w && v == x) || (u == x && v == w);
}


/* max_edge_index
 * 
 * Retorna el índice k de la arista máxima de un triángulo i, 
 * descrito por los puntos p0p1p2. Será 0 si p0p1 es máxima.
 * Será 1 si p1p2 lo es. Será 2 si p2p0 lo es.
 * */

int max_edge_index(int i, global double *r, global int *p)
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
	
	l0 = (r[2*p0 + 0] - r[2*p1 + 0])*(r[2*p0 + 0] - r[2*p1 + 0]) + (r[2*p0 + 1] - r[2*p1 + 1])*(r[2*p0 + 1] - r[2*p1 + 1]);
	l1 = (r[2*p1 + 0] - r[2*p2 + 0])*(r[2*p1 + 0] - r[2*p2 + 0]) + (r[2*p1 + 1] - r[2*p2 + 1])*(r[2*p1 + 1] - r[2*p2 + 1]);
	l2 = (r[2*p2 + 0] - r[2*p0 + 0])*(r[2*p2 + 0] - r[2*p0 + 0]) + (r[2*p2 + 1] - r[2*p0 + 1])*(r[2*p2 + 1] - r[2*p0 + 1]);
	
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





/* get_edge_index
 * 
 * Entrega el índice de la arista {u,v} respecto del triángulo i.
 * */

int get_edge_index(int u, int v, int i, global int *p)
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
		printf("** ERROR ** get_edge_index: Arista {%d,%d} no pertenece al triángulo %d.\n", u, v, i);
		/*exit(EXIT_FAILURE);*/
	}
}


/* get_common_edge
 * 
 * Entrega el índice interno de arista en común entra el triángulo i y j
 * para cada uno de los triángulos
 * */

void get_common_edge(int i, int j, int* edge_i, int* edge_j, global int *p)
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
		printf("** ERROR ** must_disconnect: Problema insperado para triángulos %d y %d.\n", i, j);
		/*exit(EXIT_FAILURE);*/
	}
	
	*edge_i = ii;
	*edge_j = ij;
}


/* is_max_max
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es máx-máx.
 * */

int is_max_max(int i, int j, global int *p, global int *max)
{

	int* edge_i;
	int* edge_j;

	get_common_edge(i, j, edge_i, edge_j, p);
	int ii = *edge_i;
	int ij = *edge_j;

	return (ij == max[j]) && (ii == max[i]);
}



/* is_nomax_nomax
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es nomáx-nomáx.
 * */

int is_nomax_nomax(int i, int j, global int *p, global int *max)
{
	int* edge_i;
	int* edge_j;
	get_common_edge(i, j, edge_i, edge_j, p);

	int ii = *edge_i;
	int ij = *edge_j;


	return (ij != max[j]) && (ii != max[i]);
}




/* must_disconnect
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es nomax-nomax o es una arista terminal que debe ser eliminada
 * */

int must_disconnect(int i, int j, global int *p, global int *max)
{
	int* edge_i;
	int* edge_j;
	get_common_edge(i, j, edge_i, edge_j, p);
	int ii = *edge_i;
	int ij = *edge_j;
	return (ii != max[i]) || (ij == max[j] && ii == max[i] && j > i);
}


