#include "consts.h"
#include "triang.cl.c"



/* initialize_values
 * 
 * Inicializa los valores de is_seed y visited.
 * */

kernel void initialize_values(global int *is_seed, global int *visited,
															global int *disconnect,
															int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		is_seed[i] = FALSE;
		visited[i] = FALSE;
		disconnect[3*i + 0] = FALSE;
		disconnect[3*i + 1] = FALSE;
		disconnect[3*i + 2] = FALSE;
	}
}

/* mark_max_max
 * 
 * Marca las aristas máx-máx.
 * */

kernel void mark_max_max(global double *r, global int *p, global int *max,
	int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		max[i] = max_edge_index(i, r, p);
	}

}


void process_edge_graph(int i, int edge_index, global int *p, global int* adj, global int* max, global int* disconnect, global int *is_seed){
	int ii;
	int ij;
	int j;
	j = adj[3*i + edge_index];
	if(j != TRIANG_BORDER){
		get_common_edge(i, j, &ii, &ij, p);
		/* If the edge must not be removed */
		if(ij != max[j] && ii != max[i]){
			disconnect[3 * i + edge_index] = TRUE;
		}
		else{
			is_seed[i] = (ij == max[j] && ii == max[i] && i < j) || is_seed[i];
		}
	}
}


/* mark_disconnections
 * 
 * Marca los arcos asociados a aristas nomáx-nomáx.
 * */

kernel void mark_disconnections(global int *p, global int *adj, global int *max,
																global int *disconnect, global int *is_seed,
																int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		process_edge_graph(i, 0, p, adj, max, disconnect, is_seed);
		process_edge_graph(i, 1, p, adj, max, disconnect, is_seed);
		process_edge_graph(i, 2, p, adj, max, disconnect, is_seed);
	}


}


/* disconnect
 * 
 * Desconecta los arcos del grafo de adyacencia, marcados para el efecto.
 * */

kernel void disconnect(global int *adj, global int *disconnect,
	int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		if(disconnect[3*i + 0] == TRUE)
		{
			adj[3*i + 0] = NO_ADJ;
		}
		
		if(disconnect[3*i + 1] == TRUE)
		{
			adj[3*i + 1] = NO_ADJ;
		}
		
		if(disconnect[3*i + 2] == TRUE)
		{
			adj[3*i + 2] = NO_ADJ;
		}
	}
	

}



/* mark_seeds
 * 
 * Marca los triángulos que son semilla.
 * */

kernel void mark_seeds(global int *p, global int *adj,
												global int *max, global int *is_seed,
												int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		is_seed[i] = ((adj[3*i + 0] != NO_ADJ) && (adj[3*i + 0] != TRIANG_BORDER) &&
				is_max_max(i, adj[3*i + 0], p, max) && (adj[3*i + 0] < i))
				||
				((adj[3*i + 1] != NO_ADJ) && (adj[3*i + 1] != TRIANG_BORDER) &&
				is_max_max(i, adj[3*i + 1], p, max) && (adj[3*i + 1] < i))
				||
				((adj[3*i + 2] != NO_ADJ) && (adj[3*i + 2] != TRIANG_BORDER) &&
				is_max_max(i, adj[3*i + 2], p, max) && (adj[3*i + 2] < i));
	}

}

/* count_class
* Counts the number of triangles per classification
*/

kernel void count_class(global int *type,
								local int *prod, global int *prod_wg,
								int class, int tnumber){
	int i = get_global_id(0);
	int il = get_local_id(0);
	int numItems = get_local_size(0);
	int wgNum = get_group_id(0);

	prod[il] = i < tnumber ? type[i] == class : 0;
	barrier(CLK_LOCAL_MEM_FENCE);



	for(int offset = 1; offset < numItems; offset *= 2){
		int mask = 2 * offset - 1;
		if((il & mask) == 0){
			//printf("wgid %i, i %i/%i, il %i/%i, off %i, il + off %i/%i, prod %i\n", wgNum, i, tnumber, il, numItems, offset, il + offset, numItems, prod[il + offset]);
			if(il + offset < numItems)
				prod[il] += prod[il + offset];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	if(il == 0)
		prod_wg[wgNum] = prod[0];
}

/* communicate_types_areas
 * 
 * Comunica a cada triángulo el área total y tipo de región contenido en su
 * nodo raíz.
 * */

kernel void communicate_types_areas(global int *type, global double *area,
																		global int *root_id,
																		int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		area[i] = area[root_id[i]];
		type[i] = type[root_id[i]];
	}
	

}
