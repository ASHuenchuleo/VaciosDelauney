#include "consts.h"
#include "triang.cl.c"




/* initialize_values
 * 
 * Inicializa los valores de is_seed y visited.
 * */

kernel void initialize_values(global int *is_seed, global int *starts_segment,
										global int *parent, global int *ordering,
										global double *area, global double *area_next,
										int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		is_seed[i] = FALSE;
		starts_segment[i] = FALSE;
		parent[i] = NO_ADJ;
		ordering[i] = i;
		area_next[i] = area[i];
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
	if(i < tnumber)
		max[i] = max_edge_index(i, r, p);
}


void process_edge(int i, int edge_index, global int *p, global int* adj,
					global int* max, global int* parent, global int* is_seed, global int* touches_border){
	int ii;
	int ij;
	int j;
	j = adj[3*i + edge_index];

	if(j != TRIANG_BORDER){
		get_common_edge(i, j, &ii, &ij, p);
		int edge_is_max_max = ij == max[j] && ii == max[i];
		is_seed[i] = (edge_is_max_max && j > i) || is_seed[i];
		/* If the edge must not be removed */
		if(!(ii != max[i]) || (edge_is_max_max && j < i)){
			parent[i] = j;
		}
	}
	else{
		touches_border[i] = TRUE;
	}


}


/* mark_disconnections
 * 
 * Disconnects edges to create the trees
 * Also identifies seeds, and if a node touches a border
 * */

kernel void mark_disconnections_tree(global int *p, global int *adj, global int *max,
																global int *is_seed,
										global int *parent, global int *succesor_next,
										global int *touches_border, global int *touches_border_next,
										int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		touches_border[i] = FALSE;
		/* If must disconnect */
		process_edge(i, 0, p, adj, max, parent, is_seed, touches_border);
		process_edge(i, 1, p, adj, max, parent, is_seed, touches_border);
		process_edge(i, 2, p, adj, max, parent, is_seed, touches_border);
		if(is_seed[i]){
			parent[i] = i;
		}
		succesor_next[i] = parent[i];
		touches_border_next[i] = touches_border[i];
	}


}

/* find_succesor
* Hace un paso de pointer jumping desde los nodos a la raiz, encontrando el sucesor
*/
kernel void find_succesor(global int *parent, global int *succesor_next, int tnumber){
	int i = get_global_id(0);
	if(i < tnumber){
		int j = parent[i];
		if(j != NO_ADJ && parent[j] != NO_ADJ)
			succesor_next[i] = parent[j];
	}

}

/* set_succesor
* Segundo paso del pointer jumping
*/
kernel void set_succesor(global int *parent, global int *succesor_next,
								int tnumber){
	int i = get_global_id(0);
	if(i < tnumber)
		parent[i] = succesor_next[i];
}

/* init_parent_copy
* Creates a copy of the parent array
*/
kernel void init_parent_copy(global int *parent, global int *parent_copy,
								int tnumber){
	int i = get_global_id(0);
	if(i < tnumber)
		parent_copy[i] = parent[i];
}

/* http://www.bealto.com/gpu-sorting_parallel-merge-local.html */
kernel void ParallelMerge_Local(global int *parent, global int *ordering, local int *aux, local int *ans, int tnumber)
{
  int ig = get_global_id(0);
  if(ig > tnumber){
  	int i = get_local_id(0); // index in workgroup
  	int wg = get_local_size(0); // workgroup size = block size, power of 2

  	// Move IN, OUT to block start
  	int offset = get_group_id(0) * wg;
  	parent += offset; ordering += offset;

  	// Load block in AUX[WG]
  	aux[i] = ordering[i];
  	// ans is originally 0, 1, 2, ... This is sorted according to ordering
  	ans[i] = ig;
  	barrier(CLK_LOCAL_MEM_FENCE); // make sure AUX is entirely up to date

  	// Now we will merge sub-sequences of length 1,2,...,WG/2
  	for (int length=1;length<wg;length<<=1)
  	{
  	  uint iKey = aux[i];
  	  int ii = i & (length-1);  // index in our sequence in 0..length-1
  	  int sibling = (i - ii) ^ length; // beginning of the sibling sequence
  	  int pos = 0;
  	  for (int inc=length;inc>0;inc>>=1) // increment for dichotomic search
  	  {
  	    int j = sibling+pos+inc-1;
  	    uint jKey = aux[j];
  	    bool smaller = (jKey < iKey) || ( jKey == iKey && j < i );
  	    pos += (smaller)?inc:0;
  	    pos = min(pos,length);
  	  }
  	  int bits = 2*length-1; // mask for destination
  	  int dest = ((ii + pos) & bits) | (i & ~bits); // destination index in merged sequence
  	  barrier(CLK_LOCAL_MEM_FENCE);
  	  aux[dest] = iKey;
  	  ans[dest] = ig;
  	  barrier(CLK_LOCAL_MEM_FENCE);
  	}

  	// Write output
  	ordering[i] = ans[i];
  }
}


/* init_linked
* Takes the node ordering saved in ordering,
* and creates the listed link in linked
*/
kernel void init_linked(global int *linked, global int *ordering,
					int tnumber){
	int i = get_global_id(0);
	if(i < tnumber){
		if(i != tnumber - 1)
			linked[ordering[i]] = ordering[i + 1];
		else
			linked[ordering[i]] = NO_ADJ;
	}

}

/* init_linked_copy
* Creates a copy of the linked list ordering
*/
kernel void init_linked_copy(global int *linked, global int *linked_copy,
								int tnumber){
	int i = get_global_id(0);
	if(i < tnumber)
		linked_copy[i] = linked[i];
}

/* mark_start_end
 * 
 * Marks in array_role wether this triangle starts or ends a tree
 * in the ordered array
 * */
kernel void mark_start_end(global int *linked, global int *parent,
								global int *starts_segment, global int *starts_segment_next, int tnumber)
{
	int i;
	i = get_global_id(0);
	if(i < tnumber){
		/* If its the end of a section */
		if(i == 0){
			starts_segment[0] = STARTS_SECTION;
			starts_segment_next[0] = STARTS_SECTION;
		}
		if(parent[i] != parent[linked[i]]){
			starts_segment[linked[i]] = STARTS_SECTION;
			starts_segment_next[linked[i]] = STARTS_SECTION;
		}
	}
}
/* find_area_jumping
 * 
 * Sets the area for the next step in prefix computation
 * */

kernel void find_area_jumping(global int *linked, global int *linked_next,
									global int *starts_segment, global int *starts_segment_next,
									global double* areas, global double *area_next,
									global int *touches_border, global int *touches_border_next,
									int tnumber)
{
	int i;
	i = get_global_id(0);
	if(i < tnumber){
		int j = linked[i];
		if(j != NO_ADJ){
			if(!starts_segment[j]){
				starts_segment_next[j] = starts_segment[i];
				area_next[j] = areas[i] +  areas[j];
				touches_border_next[j] = touches_border[i] || touches_border[j];
			}
			linked_next[i] = linked[j];
		}
	}
}

/* set_area_jumping
 * 
 * Updates the arrays of the prefix computation
 * */

kernel void set_area_jumping(global int *linked, global int *linked_next,
									global int *starts_segment, global int *starts_segment_next,
									global double* areas, global double *area_next,
									global int *touches_border, global int *touches_border_next, int tnumber)
{
	int i;
	i = get_global_id(0);
	if(i < tnumber){
		starts_segment[i] = starts_segment_next[i];
		areas[i] = area_next[i];
		linked[i] = linked_next[i];
		touches_border[i] = touches_border_next[i];
	}
}

/* communicate_data_to_root
* Finds the type of each tree and communicates that to the parent
* also counts the type of each area.
* Saves the type of each end of segment in segment_role, to later count each type
*/
kernel void communicate_data_to_root(global int* parent, global int* linked, global int* is_seed,
											global double* area, global int* type,
											global int *touches_border, global int *segment_role,
											double threshold, int tnumber){

	int i = get_global_id(0);
	if(i < tnumber){
		segment_role[i] = -2;
		if(parent[i] == -1){
			type[i] = NONZONE;
			segment_role[i] = type[i];
		}
		else if(parent[i] != NO_ADJ && (linked[i] == NO_ADJ || parent[i] != parent[linked[i]])){
			area[parent[i]] = area[i]; 

			if(is_seed[parent[i]]){
				if(area[parent[i]] >= threshold && !touches_border[i])
				{
					type[parent[i]] = INNER_VOID;
				}
				else if(area[parent[i]] >= threshold)
				{
					type[parent[i]] = BORDER_VOID;
				}
				else if(area[parent[i]] < threshold)
				{
					type[parent[i]] = WALL;
				}
				segment_role[i] = type[parent[i]];
			}
		}
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
																		global int *parent, int tnumber)
{
	int i;
	
	i = get_global_id(0);
	if(i < tnumber){
		if(parent[i] != NO_ADJ){
			area[i] = area[parent[i]];
			type[i] = type[parent[i]];
		}
	}
}
