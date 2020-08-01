/* Programa principal para O(n) mixto. */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220 

#include <stdlib.h>
#include <stdio.h>
#include "../shared/io.h"
#include "../shared/args.h"
#include "../shared/consts.h"
#include "../shared/adjgraph.h"
#include "../shared/timestamp.h"
#include "ocl.h"

#include <CL/cl.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#define KERNELS_FILE_PATH "kernels.cl.c"

const char *getErrorString(cl_int error)
{
	switch(error){
	    /* run-time and JIT compiler errors */
	    case 0: return "CL_SUCCESS";
	    case -1: return "CL_DEVICE_NOT_FOUND";
	    case -2: return "CL_DEVICE_NOT_AVAILABLE";
	    case -3: return "CL_COMPILER_NOT_AVAILABLE";
	    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	    case -5: return "CL_OUT_OF_RESOURCES";
	    case -6: return "CL_OUT_OF_HOST_MEMORY";
	    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	    case -8: return "CL_MEM_COPY_OVERLAP";
	    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	    case -11: return "CL_BUILD_PROGRAM_FAILURE";
	    case -12: return "CL_MAP_FAILURE";
	    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	    case -16: return "CL_LINKER_NOT_AVAILABLE";
	    case -17: return "CL_LINK_PROGRAM_FAILURE";
	    case -18: return "CL_DEVICE_PARTITION_FAILED";
	    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

	    /* compile-time errors */
	    case -30: return "CL_INVALID_VALUE";
	    case -31: return "CL_INVALID_DEVICE_TYPE";
	    case -32: return "CL_INVALID_PLATFORM";
	    case -33: return "CL_INVALID_DEVICE";
	    case -34: return "CL_INVALID_CONTEXT";
	    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	    case -36: return "CL_INVALID_COMMAND_QUEUE";
	    case -37: return "CL_INVALID_HOST_PTR";
	    case -38: return "CL_INVALID_MEM_OBJECT";
	    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	    case -40: return "CL_INVALID_IMAGE_SIZE";
	    case -41: return "CL_INVALID_SAMPLER";
	    case -42: return "CL_INVALID_BINARY";
	    case -43: return "CL_INVALID_BUILD_OPTIONS";
	    case -44: return "CL_INVALID_PROGRAM";
	    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	    case -46: return "CL_INVALID_KERNEL_NAME";
	    case -47: return "CL_INVALID_KERNEL_DEFINITION";
	    case -48: return "CL_INVALID_KERNEL";
	    case -49: return "CL_INVALID_ARG_INDEX";
	    case -50: return "CL_INVALID_ARG_VALUE";
	    case -51: return "CL_INVALID_ARG_SIZE";
	    case -52: return "CL_INVALID_KERNEL_ARGS";
	    case -53: return "CL_INVALID_WORK_DIMENSION";
	    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	    case -56: return "CL_INVALID_GLOBAL_OFFSET";
	    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	    case -58: return "CL_INVALID_EVENT";
	    case -59: return "CL_INVALID_OPERATION";
	    case -60: return "CL_INVALID_GL_OBJECT";
	    case -61: return "CL_INVALID_BUFFER_SIZE";
	    case -62: return "CL_INVALID_MIP_LEVEL";
	    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	    case -64: return "CL_INVALID_PROPERTY";
	    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	    case -66: return "CL_INVALID_COMPILER_OPTIONS";
	    case -67: return "CL_INVALID_LINKER_OPTIONS";
	    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

	    /* extension errors */
	    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	    default: return "Unknown OpenCL error";
		}
}

void retDebug(char* name, cl_int ret){
	#ifdef DEBUG
	printf("%s: %s\n", name, getErrorString(ret));
	#endif
	if(ret != 0){
		printf("%s: %s\n", name, getErrorString(ret));
		exit(1);
	}
}


void mergeInt(int* arr, int* crit, int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
    /* create temp arrays */
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    int *L_crit = malloc(n1 * sizeof(int));
    int *R_crit = malloc(n2 * sizeof(int));
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 

    for (i = 0; i < n1; i++){
        L_crit[i] = crit[l + i];
    }
    for (j = 0; j < n2; j++) 
        R_crit[j] = crit[m + 1 + j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; 
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) { 
        if (L_crit[i] <= R_crit[j]) { 
            arr[k] = L[i]; 
            crit[k] = L_crit[i];
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            crit[k] = R_crit[j];
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) { 
        arr[k] = L[i]; 
        crit[k] = L_crit[i];
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) { 
        arr[k] = R[j]; 
        crit[k] = R_crit[j];
        j++; 
        k++; 
    }
    free(L);
    free(R);
    free(L_crit);
    free(R_crit);
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSortInt(int* arr, int* crit, int l, int r) 
{ 
    if (l < r) { 
        int m = l + (r - l) / 2; 
  
        mergeSortInt(arr, crit, l, m); 
        mergeSortInt(arr, crit, m + 1, r); 
  
        mergeInt(arr, crit, l, m, r); 
    } 
} 

int main(int argc, char **argv)
{

	int pnumber;
	int tnumber;
	double *r;
	int *p;
	int *adj;

	int *parent;
	int *parent_copy;

	int *linked;
	int *linked_copy;

	double *area;
	double *area_next;

	int *max;
	int *is_seed;
	int *jumping_next;

	int *segment_role;
	int *segment_role_next;

	int *touches_border;
	int *touches_border_next;

	int *type;

	int *num_nonzones_prods_wg;
	int *num_ivoids_prods_wg;
	int *num_bvoids_prods_wg;
	int *num_walls_prods_wg;
	int *num_nonzone_triangs_prods_wg;
	int *num_ivoid_triangs_prods_wg;
	int *num_bvoid_triangs_prods_wg;
	int *num_wall_triangs_prods_wg;


	char *ppath;
	double threshold;
	char *cpath_prefix;


	struct timeval t;
	
	start_time_measure(&t);


	
	/* Preparativos para alineación de memoria. */
	int alignment = 4096;
	void *align_settings[2] = {&alignment, &upper_next_multiple64};
	int (*new_aligned_mem_size)(int);
	
	read_arguments(argc, argv, &ppath, &threshold, &cpath_prefix);
	
	print_timestamp("Ejecutando qdelaunay...\n", t);
	read_qdelaunay_data(ppath, &r, &p, &adj, &area, &pnumber, &tnumber, align_settings);
	
	printf("* %d triángulos\n", tnumber);
	printf("* %d puntos\n", pnumber);
	print_timestamp("Preparando para procesamiento paralelo...\n", t);

	/* configuracion de work groups */
	/* Trabajar en una dimensión. */
	size_t localSize[1] = {pow(2, 7)};
	size_t numLocalGroups = ceil(((float)tnumber)/localSize[0]);

	size_t global_work_size[1] = {localSize[0] * numLocalGroups}; /* <--- POR CHEQUEAR SI SUPERA MAXIMO */

	printf("%ld work items per workgroup, %ld workgroups, %ld total work items\n", localSize[0], numLocalGroups, global_work_size[0]);
	
	/* Reservar memoria alineada. */
	new_aligned_mem_size = align_settings[1];

	posix_memalign((void **)&max, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&is_seed, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&jumping_next, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	posix_memalign((void **)&segment_role, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&segment_role_next, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	posix_memalign((void **)&touches_border, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&touches_border_next, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	posix_memalign((void **)&type, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&parent, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&parent_copy, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	posix_memalign((void **)&linked, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&linked_copy, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	posix_memalign((void **)&area_next, alignment, new_aligned_mem_size(tnumber*sizeof(double)));

	posix_memalign((void **)&num_nonzones_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_ivoids_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_bvoids_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_walls_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_nonzone_triangs_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_ivoid_triangs_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_bvoid_triangs_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));
	posix_memalign((void **)&num_wall_triangs_prods_wg, alignment, new_aligned_mem_size(numLocalGroups*sizeof(int)));

	/* Preparaciones para OpenCL. */
	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_context context;
	cl_command_queue command_queue;
	cl_mem memobj_r;
	cl_mem memobj_p;
	cl_mem memobj_adj;
	cl_mem memobj_max;
	cl_mem memobj_is_seed;

	cl_mem memobj_jumping_next;
	cl_mem memobj_area;
	cl_mem memobj_area_next;

	cl_mem memobj_segment_role;
	cl_mem memobj_segment_role_next;

	cl_mem memobj_touches_border;
	cl_mem memobj_touches_border_next;

	cl_mem memobj_type;

	cl_mem memobj_parent;
	cl_mem memobj_parent_copy;

	cl_mem memobj_linked;
	cl_mem memobj_linked_copy;

	cl_mem memobj_num_nonzones_prods_wg;
	cl_mem memobj_num_ivoids_prods_wg;
	cl_mem memobj_num_bvoids_prods_wg;
	cl_mem memobj_num_walls_prods_wg;
	cl_mem memobj_num_nonzone_triangs_prods_wg;
	cl_mem memobj_num_ivoid_triangs_prods_wg;
	cl_mem memobj_num_bvoid_triangs_prods_wg;
	cl_mem memobj_num_wall_triangs_prods_wg;


	cl_program program;

	cl_kernel init_values_kernel;
	cl_kernel mark_max_kernel;
	cl_kernel create_tree_kernel;
	cl_kernel find_succesor_kernel;
	cl_kernel set_succesor_kernel;

	cl_kernel init_parent_copy_kernel;

	cl_kernel mergesort_kernel;

	cl_kernel init_linked_kernel;
	cl_kernel init_linked_copy_kernel;

	cl_kernel mark_end_nodes_kernel;
	cl_kernel find_area_jumping_kernel;
	cl_kernel set_area_jumping_kernel;
	cl_kernel communicate_data_to_root_kernel;
	cl_kernel communicate_types_areas_kernel;

	cl_kernel count_class_kernel;

	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	char dev_name[1024];
	void *map_type;
	void *map_area;
	void *map_parent_copy;
	void *map_linked_copy;

	#ifdef DEBUG
	void *map_segment_role;
	void *map_is_seed;
	void *map_max;
	void *map_parent;
	void *map_linked;
	void *map_touches_border;
	#endif

	void *map_num_nonzones_prods_wg;
	void *map_num_ivoids_prods_wg;
	void *map_num_bvoids_prods_wg;
	void *map_num_walls_prods_wg;
	void *map_num_nonzone_triangs_prods_wg;
	void *map_num_ivoid_triangs_prods_wg;
	void *map_num_bvoid_triangs_prods_wg;
	void *map_num_wall_triangs_prods_wg;


		
	/* Obtener información de plataforma/dispositivo. */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(dev_name), dev_name, NULL);
	
	  /*	printf("* Trabajando con: %s\n, número de plataformas %d %d", dev_name, ret_num_devices, ret_num_devices);*/
	  printf("* Trabajando con: %s\n", dev_name);

	  cl_uint native_double_width;    
	  clGetDeviceInfo(device_id, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &native_double_width, NULL);

	  if(native_double_width == 0){
	    printf("No double precision support.\n");
	  }else{
	    printf("Double precision support.\n");
	  }

	cl_ulong localMemSize = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, 
	    sizeof(cl_ulong), &localMemSize, NULL);
	printf("Local memory size: %li integers\n", localMemSize/sizeof(int));


	/* Crear contexto de OpenCL. */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	
	/* Crear cola de comandos. */
	/*command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);*/
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Crear búferes de memoria. */
	memobj_r = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 2 * sizeof(double) * pnumber, r, &ret);
	memobj_p = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(int) * tnumber, p, &ret);
	memobj_adj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(int) * tnumber, adj, &ret);
	memobj_max = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, max, &ret);
	memobj_is_seed = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, is_seed, &ret);
	memobj_jumping_next = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, jumping_next, &ret);

	memobj_area = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(double) * tnumber, area, &ret);
	memobj_area_next = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(double) * tnumber, area_next, &ret);

	memobj_segment_role = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, segment_role, &ret);
	memobj_segment_role_next = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, segment_role_next, &ret);

	memobj_touches_border = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, touches_border, &ret);
	memobj_touches_border_next = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, touches_border_next, &ret);

	memobj_type = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, type, &ret);

	memobj_parent = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, parent, &ret);
	memobj_parent_copy = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, parent_copy, &ret);

	memobj_linked = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, linked, &ret);
	memobj_linked_copy = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, linked_copy, &ret);

	memobj_num_nonzones_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_nonzones_prods_wg, &ret);
	memobj_num_ivoids_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_ivoids_prods_wg, &ret);
	memobj_num_bvoids_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_bvoids_prods_wg, &ret);
	memobj_num_walls_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_walls_prods_wg, &ret);
	memobj_num_nonzone_triangs_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_nonzone_triangs_prods_wg, &ret);
	memobj_num_ivoid_triangs_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_ivoid_triangs_prods_wg, &ret);
	memobj_num_bvoid_triangs_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_bvoid_triangs_prods_wg, &ret);
	memobj_num_wall_triangs_prods_wg = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * numLocalGroups, num_wall_triangs_prods_wg, &ret);

	
	/* Crear programa. */
	program = create_program(context, KERNELS_FILE_PATH, device_id);
	
	/* Crear kerneles. */
	init_values_kernel = clCreateKernel(program, "initialize_values", &ret);
	mark_max_kernel = clCreateKernel(program, "mark_max_max", &ret);
	create_tree_kernel = clCreateKernel(program, "mark_disconnections_tree", &ret);
	find_succesor_kernel = clCreateKernel(program, "find_succesor", &ret);
	set_succesor_kernel = clCreateKernel(program, "set_succesor", &ret);

	mergesort_kernel = clCreateKernel(program, "ParallelMerge_Local", &ret);

	init_parent_copy_kernel = clCreateKernel(program, "init_parent_copy", &ret);

	init_linked_kernel = clCreateKernel(program, "init_linked", &ret);
	init_linked_copy_kernel = clCreateKernel(program, "init_linked_copy", &ret);

	mark_end_nodes_kernel = clCreateKernel(program, "mark_start_end", &ret);
	find_area_jumping_kernel = clCreateKernel(program, "find_area_jumping", &ret);
	set_area_jumping_kernel = clCreateKernel(program, "set_area_jumping", &ret);
	communicate_data_to_root_kernel = clCreateKernel(program, "communicate_data_to_root", &ret);
	communicate_types_areas_kernel = clCreateKernel(program, "communicate_types_areas", &ret);

	count_class_kernel = clCreateKernel(program, "count_class", &ret);

	
	/* Establecer argumentos para kerneles. */
	ret = clSetKernelArg(init_values_kernel, 0, sizeof(cl_mem), (void *)&memobj_is_seed);
	ret = clSetKernelArg(init_values_kernel, 1, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(init_values_kernel, 2, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(init_values_kernel, 3, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(init_values_kernel, 4, sizeof(cl_mem), (void *)&memobj_area);
	ret = clSetKernelArg(init_values_kernel, 5, sizeof(cl_mem), (void *)&memobj_area_next);
	ret = clSetKernelArg(init_values_kernel, 6, sizeof(int), &tnumber);
	
	ret = clSetKernelArg(mark_max_kernel, 0, sizeof(cl_mem), (void *)&memobj_r);
	ret = clSetKernelArg(mark_max_kernel, 1, sizeof(cl_mem), (void *)&memobj_p);
	ret = clSetKernelArg(mark_max_kernel, 2, sizeof(cl_mem), (void *)&memobj_max);
	ret = clSetKernelArg(mark_max_kernel, 3, sizeof(int), &tnumber);

	
	ret = clSetKernelArg(create_tree_kernel, 0, sizeof(cl_mem), (void *)&memobj_p);
	ret = clSetKernelArg(create_tree_kernel, 1, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(create_tree_kernel, 2, sizeof(cl_mem), (void *)&memobj_max);
	ret = clSetKernelArg(create_tree_kernel, 3, sizeof(cl_mem), (void *)&memobj_is_seed);
	ret = clSetKernelArg(create_tree_kernel, 4, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(create_tree_kernel, 5, sizeof(cl_mem), (void *)&memobj_jumping_next);
	ret = clSetKernelArg(create_tree_kernel, 6, sizeof(cl_mem), (void *)&memobj_touches_border);
	ret = clSetKernelArg(create_tree_kernel, 7, sizeof(cl_mem), (void *)&memobj_touches_border_next);
	ret = clSetKernelArg(create_tree_kernel, 8, sizeof(int), &tnumber);


	ret = clSetKernelArg(find_succesor_kernel, 0, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(find_succesor_kernel, 1, sizeof(cl_mem), (void *)&memobj_jumping_next);
	ret = clSetKernelArg(find_succesor_kernel, 2, sizeof(int), &tnumber);


	ret = clSetKernelArg(set_succesor_kernel, 0, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(set_succesor_kernel, 1, sizeof(cl_mem), (void *)&memobj_jumping_next);
	ret = clSetKernelArg(set_succesor_kernel, 2, sizeof(int), &tnumber);


	ret = clSetKernelArg(mergesort_kernel, 0, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(mergesort_kernel, 1, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(mergesort_kernel, 2, sizeof(int) * localSize[0], NULL);
	ret = clSetKernelArg(mergesort_kernel, 3, sizeof(int) * localSize[0], NULL);
	ret = clSetKernelArg(mergesort_kernel, 4, sizeof(int), &tnumber);




	ret = clSetKernelArg(init_parent_copy_kernel, 0, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(init_parent_copy_kernel, 1, sizeof(cl_mem), (void *)&memobj_parent_copy);
	ret = clSetKernelArg(init_parent_copy_kernel, 2, sizeof(int), &tnumber);


	ret = clSetKernelArg(init_linked_kernel, 0, sizeof(cl_mem), (void *)&memobj_linked);
	ret = clSetKernelArg(init_linked_kernel, 1, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(init_linked_kernel, 2, sizeof(int), &tnumber);


	ret = clSetKernelArg(init_linked_copy_kernel, 0, sizeof(cl_mem), (void *)&memobj_linked);
	ret = clSetKernelArg(init_linked_copy_kernel, 1, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(init_linked_copy_kernel, 2, sizeof(int), &tnumber);


	ret = clSetKernelArg(mark_end_nodes_kernel, 0, sizeof(cl_mem), (void *)&memobj_linked);
	ret = clSetKernelArg(mark_end_nodes_kernel, 1, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(mark_end_nodes_kernel, 2, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(mark_end_nodes_kernel, 3, sizeof(cl_mem), (void *)&memobj_segment_role_next);
	ret = clSetKernelArg(mark_end_nodes_kernel, 4, sizeof(int), &tnumber);

	ret = clSetKernelArg(find_area_jumping_kernel, 0, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(find_area_jumping_kernel, 1, sizeof(cl_mem), (void *)&memobj_jumping_next);
	ret = clSetKernelArg(find_area_jumping_kernel, 2, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(find_area_jumping_kernel, 3, sizeof(cl_mem), (void *)&memobj_segment_role_next);
	ret = clSetKernelArg(find_area_jumping_kernel, 4, sizeof(cl_mem), (void *)&memobj_area);
	ret = clSetKernelArg(find_area_jumping_kernel, 5, sizeof(cl_mem), (void *)&memobj_area_next);
	ret = clSetKernelArg(find_area_jumping_kernel, 6, sizeof(cl_mem), (void *)&memobj_touches_border);
	ret = clSetKernelArg(find_area_jumping_kernel, 7, sizeof(cl_mem), (void *)&memobj_touches_border_next);
	ret = clSetKernelArg(find_area_jumping_kernel, 8, sizeof(int), &tnumber);


	ret = clSetKernelArg(set_area_jumping_kernel, 0, sizeof(cl_mem), (void *)&memobj_linked_copy);
	ret = clSetKernelArg(set_area_jumping_kernel, 1, sizeof(cl_mem), (void *)&memobj_jumping_next);
	ret = clSetKernelArg(set_area_jumping_kernel, 2, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(set_area_jumping_kernel, 3, sizeof(cl_mem), (void *)&memobj_segment_role_next);
	ret = clSetKernelArg(set_area_jumping_kernel, 4, sizeof(cl_mem), (void *)&memobj_area);
	ret = clSetKernelArg(set_area_jumping_kernel, 5, sizeof(cl_mem), (void *)&memobj_area_next);
	ret = clSetKernelArg(set_area_jumping_kernel, 6, sizeof(cl_mem), (void *)&memobj_touches_border);
	ret = clSetKernelArg(set_area_jumping_kernel, 7, sizeof(cl_mem), (void *)&memobj_touches_border_next);
	ret = clSetKernelArg(set_area_jumping_kernel, 8, sizeof(int), &tnumber);


	ret = clSetKernelArg(communicate_data_to_root_kernel, 0, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 1, sizeof(cl_mem), (void *)&memobj_linked);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 2, sizeof(cl_mem), (void *)&memobj_is_seed);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 3, sizeof(cl_mem), (void *)&memobj_area);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 4, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 5, sizeof(cl_mem), (void *)&memobj_touches_border);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 6, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 7, sizeof(double), &threshold);
	ret = clSetKernelArg(communicate_data_to_root_kernel, 8, sizeof(int), &tnumber);



	ret = clSetKernelArg(communicate_types_areas_kernel, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(communicate_types_areas_kernel, 1, sizeof(cl_mem), (void *)&memobj_area);
	ret = clSetKernelArg(communicate_types_areas_kernel, 2, sizeof(cl_mem), (void *)&memobj_parent);
	ret = clSetKernelArg(communicate_types_areas_kernel, 3, sizeof(int), &tnumber);


	ret = clSetKernelArg(count_class_kernel, 1, sizeof(int) * localSize[0], NULL);
	ret = clSetKernelArg(count_class_kernel, 4, sizeof(int), &tnumber);


	clFinish(command_queue);



	

	print_timestamp("Ejecutando primera fase de kerneles...\n", t);

	

	/* Encolar kerneles en cola de comandos. */
	ret = clEnqueueNDRangeKernel(command_queue, init_values_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("init_values_kernel", ret);
	
	ret = clEnqueueNDRangeKernel(command_queue, mark_max_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("mark_max_kernel", ret);
	
	ret = clEnqueueNDRangeKernel(command_queue, create_tree_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("create_tree_kernel", ret);
		
	int step_count;
	/* Calcular el numero de pasos para llegar a la raiz */
	int jumping_max_steps = ceil(log(tnumber)/log(2.));
	for(step_count = 0; step_count < jumping_max_steps; step_count++){
		ret = clEnqueueNDRangeKernel(command_queue, find_succesor_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
		/*printf("find_succesor_kernel: %s\n", getErrorString(ret));*/

		ret = clEnqueueNDRangeKernel(command_queue, set_succesor_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
		/*printf("set_succesor_kernel: %s\n", getErrorString(ret));*/
	}

	ret = clEnqueueNDRangeKernel(command_queue, init_parent_copy_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("init_parent_copy_kernel", ret);

	ret = clEnqueueNDRangeKernel(command_queue, mergesort_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("mergesort_kernel", ret);
	
	
	
	clFinish(command_queue);


	/*** DEBUG ***/
	#ifdef DEBUG

	map_is_seed = clEnqueueMapBuffer(command_queue, memobj_is_seed, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);
	
	retDebug("Reade is_seed", ret);
	

	map_parent = clEnqueueMapBuffer(command_queue, memobj_parent, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);

	map_max = clEnqueueMapBuffer(command_queue, memobj_max, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);

	map_area = clEnqueueMapBuffer(command_queue, memobj_area, CL_TRUE, CL_MAP_READ,
																	0, sizeof(double) * tnumber, 0, NULL, NULL,  &ret);


	clEnqueueUnmapMemObject(command_queue, memobj_parent, map_parent, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_max, map_max, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_area, map_area, 0, NULL, NULL);
	clFinish(command_queue);


 	unsigned long e;
	int adj_mat[tnumber][tnumber];
	int k;
	int j;
	for(k = 0; k < tnumber; k++){
		for(j = 0; j < tnumber; j++)
		{
			adj_mat[k][j] = 0;
		}
	}
	for(e = 0; e < tnumber;  e++){
		if(parent[e] > -1)
			adj_mat[e][parent[e]] = 1;
		if(parent[e] > -1)
			adj_mat[e][parent[e]] = 1;
		if(parent[e] > -1)
			adj_mat[e][parent[e]] = 1;
	}
	FILE* f = fopen("adjacency.txt", "w");
	for(k = 0; k < tnumber; k++){
		for(j = 0; j < tnumber; j++)
		{	
			if(j == tnumber - 1)
				fprintf(f,"%i\n", adj_mat[k][j]);
			else
				fprintf(f,"%i,", adj_mat[k][j]);
		}
	}
	fclose(f);
	#endif
	/*** DEBUG ***/

	print_timestamp("Sorting by parent sequentially...\n", t);

	/* Ordenar */
	/*
	map_linked_copy = clEnqueueMapBuffer(command_queue, memobj_linked_copy, CL_TRUE, CL_MAP_WRITE,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);

	map_parent_copy = clEnqueueMapBuffer(command_queue, memobj_parent_copy, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);


	mergeSortInt(linked_copy, parent_copy, 0, tnumber - 1);

	clEnqueueUnmapMemObject(command_queue, memobj_linked_copy, map_linked_copy, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_parent_copy, map_parent_copy, 0, NULL, NULL);
	clFinish(command_queue);
	*/
	print_timestamp("Ejecutando última fase de kerneles...\n", t);

	ret = clEnqueueNDRangeKernel(command_queue, init_linked_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("init_linked_kernel", ret);
	


	ret = clEnqueueNDRangeKernel(command_queue, init_linked_copy_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("init_linked_copy_kernel", ret);
	

	ret = clEnqueueNDRangeKernel(command_queue, mark_end_nodes_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("mark_end_nodes_kernel", ret);
	

	for(step_count = 0; step_count < jumping_max_steps; step_count++){
		ret = clEnqueueNDRangeKernel(command_queue, find_area_jumping_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
		ret = clEnqueueNDRangeKernel(command_queue, set_area_jumping_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	}
	ret = clEnqueueNDRangeKernel(command_queue, communicate_data_to_root_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("communicate_data_to_root_kernel", ret);
	

	ret = clEnqueueNDRangeKernel(command_queue, communicate_types_areas_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("communicate_types_areas_kernel", ret);
	



	cl_int class_to_count;


	class_to_count = NONZONE;
	ret = clSetKernelArg(count_class_kernel, 3, sizeof(cl_int), &class_to_count);

	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_nonzone_triangs_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_nonzones_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	class_to_count = INNER_VOID;
	ret = clSetKernelArg(count_class_kernel, 3, sizeof(cl_int), &class_to_count);

	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_ivoid_triangs_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_ivoids_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	class_to_count = BORDER_VOID;
	ret = clSetKernelArg(count_class_kernel, 3, sizeof(cl_int), &class_to_count);

	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_bvoid_triangs_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_bvoids_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	class_to_count = WALL;
	ret = clSetKernelArg(count_class_kernel, 3, sizeof(cl_int), &class_to_count);

	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_wall_triangs_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	
	ret = clSetKernelArg(count_class_kernel, 0, sizeof(cl_mem), (void *)&memobj_segment_role);
	ret = clSetKernelArg(count_class_kernel, 2, sizeof(cl_mem), (void *)&memobj_num_walls_prods_wg);
	ret = clEnqueueNDRangeKernel(command_queue, count_class_kernel, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	retDebug("count_class_kernel", ret);
	


	clFinish(command_queue);

	/*** DEBUG ***/
	#ifdef DEBUG
	map_linked = clEnqueueMapBuffer(command_queue, memobj_linked, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);

	map_segment_role = clEnqueueMapBuffer(command_queue, memobj_segment_role, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);

	map_is_seed = clEnqueueMapBuffer(command_queue, memobj_type, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);

	map_touches_border = clEnqueueMapBuffer(command_queue, memobj_touches_border, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);

	clEnqueueUnmapMemObject(command_queue, memobj_linked, map_linked, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_segment_role, map_segment_role, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_is_seed, map_is_seed, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_touches_border, map_touches_border, 0, NULL, NULL);
	clFinish(command_queue);

	e = 0;
	while(TRUE){
		if(e == NO_ADJ)
			break;
		printf("%li (root %i, is seed = %i), type = %i, border = %i, area = %f\n", e, parent[e], is_seed[e],type[e], touches_border[e],area[e]);
		e = linked[e];
	}

	for(e = 0; e < tnumber;  e++){
		printf("%i ", type[e]);
	}
	printf("\n");
	printf("\n");
	#endif
	/*** DEBUG ***/


	/* Encolar regreso de datos. */

	map_area = clEnqueueMapBuffer(command_queue, memobj_area, CL_TRUE, CL_MAP_READ,
																	0, sizeof(double) * tnumber, 0, NULL, NULL,  &ret);
	map_type = clEnqueueMapBuffer(command_queue, memobj_type, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);


	map_num_nonzone_triangs_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_nonzone_triangs_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_ivoid_triangs_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_ivoid_triangs_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_bvoid_triangs_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_bvoid_triangs_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_wall_triangs_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_wall_triangs_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);


	map_num_nonzones_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_nonzones_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_ivoids_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_ivoids_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_bvoids_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_bvoids_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	map_num_walls_prods_wg = clEnqueueMapBuffer(command_queue, memobj_num_walls_prods_wg, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * numLocalGroups, 0, NULL, NULL, &ret);

	clEnqueueUnmapMemObject(command_queue, memobj_area, map_area, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_type, map_type, 0, NULL, NULL);

	clEnqueueUnmapMemObject(command_queue, memobj_num_nonzone_triangs_prods_wg, map_num_nonzone_triangs_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_ivoid_triangs_prods_wg, map_num_ivoid_triangs_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_bvoid_triangs_prods_wg, map_num_bvoid_triangs_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_wall_triangs_prods_wg, map_num_wall_triangs_prods_wg, 0, NULL, NULL);

	clEnqueueUnmapMemObject(command_queue, memobj_num_nonzones_prods_wg, map_num_nonzones_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_ivoids_prods_wg, map_num_ivoids_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_bvoids_prods_wg, map_num_bvoids_prods_wg, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_num_walls_prods_wg, map_num_walls_prods_wg, 0, NULL, NULL);



	clFinish(command_queue);

	int num_nonzones;
	int num_ivoids;
	int num_bvoids;
	int num_walls;
	int num_nonzone_triangs;
	int num_ivoid_triangs;
	int num_bvoid_triangs;
	int num_wall_triangs;
	
	num_nonzones = 0;
	num_ivoids = 0;
	num_bvoids = 0;
	num_walls = 0;
	num_nonzone_triangs = 0;
	num_ivoid_triangs = 0;
	num_bvoid_triangs = 0;
	num_wall_triangs = 0;

	int i;
	print_timestamp("Analizando zonas..\n", t);
	for(i = 0; i < numLocalGroups; i++)
	{	
		num_nonzones += num_nonzones_prods_wg[i];
		num_ivoids += num_ivoids_prods_wg[i];
		num_bvoids += num_bvoids_prods_wg[i];
		num_walls += num_walls_prods_wg[i];

		num_ivoid_triangs += num_ivoid_triangs_prods_wg[i];
		num_bvoid_triangs += num_bvoid_triangs_prods_wg[i];
		num_wall_triangs += num_wall_triangs_prods_wg[i];
		num_nonzone_triangs += num_nonzone_triangs_prods_wg[i];
	}
	/*
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
			if(type[i] == INNER_VOID)
			{
				num_ivoids++;
				num_ivoid_triangs = num_ivoid_triangs + 1;
			}
			else if(type[i] == BORDER_VOID)
			{
				num_bvoids++;
				num_bvoid_triangs = num_bvoid_triangs + 1;
			}
			else if(type[i] == WALL)
			{
				num_walls++;
				num_wall_triangs = num_wall_triangs + 1;
			}
			else
			{
				num_nonzones++;
				num_nonzone_triangs = num_nonzone_triangs + 1;
			}
	}
	*/

	
	printf("* Número de vacíos internos: %d (%d triángulos)\n", num_ivoids, num_ivoid_triangs);
	printf("* Número de vacíos de borde: %d (%d triángulos)\n", num_bvoids, num_bvoid_triangs);
	printf("* Número de murallas: %d (%d triángulos)\n", num_walls, num_wall_triangs);
	printf("* Número de no-zonas: %d (%d triángulos)\n", num_nonzones, num_nonzone_triangs);
	
	print_timestamp("Escribiendo datos...\n", t);
	
	write_classification(cpath_prefix, jumping_next, type, area, tnumber, num_nonzone_triangs,
												num_ivoid_triangs, num_bvoid_triangs, num_wall_triangs);
	
	/* Liberar memoria. */
	free(r);
	free(p);
	free(adj);
	free(area);
	free(max);
	free(is_seed);
	free(jumping_next);
	free(segment_role);
	free(type);


	free(segment_role_next);

	free(touches_border);
	free(touches_border_next);

	free(parent);
	free(parent_copy);

	free(linked);
	free(linked_copy);

	free(area_next);

	free(num_nonzones_prods_wg);
	free(num_ivoids_prods_wg);
	free(num_bvoids_prods_wg);
	free(num_walls_prods_wg);
	free(num_nonzone_triangs_prods_wg);
	free(num_ivoid_triangs_prods_wg);
	free(num_bvoid_triangs_prods_wg);
	free(num_wall_triangs_prods_wg);

	clReleaseMemObject(memobj_r);
	clReleaseMemObject(memobj_p);
	clReleaseMemObject(memobj_adj);
	clReleaseMemObject(memobj_max);
	clReleaseMemObject(memobj_is_seed);
	clReleaseMemObject(memobj_jumping_next);
	clReleaseMemObject(memobj_area);
	clReleaseMemObject(memobj_area_next);
	clReleaseMemObject(memobj_segment_role);
	clReleaseMemObject(memobj_segment_role_next);
	clReleaseMemObject(memobj_touches_border);
	clReleaseMemObject(memobj_touches_border_next);
	clReleaseMemObject(memobj_type);
	clReleaseMemObject(memobj_parent);
	clReleaseMemObject(memobj_parent_copy);
	clReleaseMemObject(memobj_linked);
	clReleaseMemObject(memobj_linked_copy);
	clReleaseMemObject(memobj_num_nonzones_prods_wg);
	clReleaseMemObject(memobj_num_ivoids_prods_wg);
	clReleaseMemObject(memobj_num_bvoids_prods_wg);
	clReleaseMemObject(memobj_num_walls_prods_wg);
	clReleaseMemObject(memobj_num_nonzone_triangs_prods_wg);
	clReleaseMemObject(memobj_num_ivoid_triangs_prods_wg);
	clReleaseMemObject(memobj_num_bvoid_triangs_prods_wg);
	clReleaseMemObject(memobj_num_wall_triangs_prods_wg);
	
	print_timestamp("Fin.\n", t);
	
	return EXIT_SUCCESS;
}
