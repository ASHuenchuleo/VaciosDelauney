/* Programa principal para O(n) mixto. */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 120

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
  

int main(int argc, char **argv)
{
	int pnumber;
	int tnumber;
	double *r;
	int *p;
	int *adj;
	double *area;
	int *max;
	int *is_seed;
	int *root_id;
	int *visited;
	int *disconnect;
	int *type;
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
	
	new_aligned_mem_size = align_settings[1];
	
	/* Reservar memoria alineada. */
	posix_memalign((void **)&max, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&is_seed, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&root_id, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&visited, alignment, new_aligned_mem_size(tnumber*sizeof(int)));
	posix_memalign((void **)&disconnect, alignment, new_aligned_mem_size(3*tnumber*sizeof(int)));
	posix_memalign((void **)&type, alignment, new_aligned_mem_size(tnumber*sizeof(int)));

	/* Preparaciones para OpenCL. */
	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_context context;
	cl_command_queue command_queue;
	cl_mem memobj_r;
	cl_mem memobj_p;
	cl_mem memobj_max;
	cl_mem memobj_is_seed;
	cl_mem memobj_root_id;
	cl_mem memobj_area;
	cl_mem memobj_visited;
	cl_mem memobj_disconnect;
	cl_mem memobj_type;
	cl_mem memobj_adj;
	cl_program program;
	cl_kernel kernel0;
	cl_kernel kernel1;
	cl_kernel kernel2;
	cl_kernel kernel3;
	cl_kernel kernel5;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	char dev_name[1024];
	void *map_max;
	void *map_type;
	void *map_area;
	void *map_root_id;
	void *map_is_seed;
	void *map_adj;


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


	/* Crear contexto de OpenCL. */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	
	/* Crear cola de comandos. */
	/*command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);*/
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* configuracion de work groups */
	/* Trabajar en una dimensión. */
	size_t localSize[1] = {tnumber};
	size_t numLocalGroups = ceil(((float)tnumber)/localSize[0]);

	size_t global_work_size[1] = {localSize[0] * numLocalGroups}; /* <--- POR CHEQUEAR SI SUPERA MAXIMO */

	printf("%ld work items per workgroup, %ld workgroups, %ld total work items\n", localSize[0], numLocalGroups, global_work_size[0]);
	
	/* Crear búferes de memoria. */
	memobj_r = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 2 * sizeof(double) * pnumber, r, &ret);
	memobj_p = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(int) * tnumber, p, &ret);
	memobj_adj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(int) * tnumber, adj, &ret);
	memobj_max = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, max, &ret);
	memobj_is_seed = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, is_seed, &ret);
	memobj_root_id = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, root_id, &ret);
	memobj_area = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(double) * tnumber, area, &ret);
	memobj_visited = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, visited, &ret);
	memobj_disconnect = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(int) * tnumber, disconnect, &ret);
	memobj_type = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(int) * tnumber, type, &ret);
	
	/* Crear programa. */
	program = create_program(context, KERNELS_FILE_PATH, device_id);
	
	/* Crear kerneles. */
	kernel0 = clCreateKernel(program, "initialize_values", &ret);
	kernel1 = clCreateKernel(program, "mark_max_max", &ret);
	kernel2 = clCreateKernel(program, "mark_disconnections", &ret);
	kernel3 = clCreateKernel(program, "disconnect", &ret);
	kernel5 = clCreateKernel(program, "communicate_types_areas", &ret);
	
	/* Establecer argumentos para kerneles. */
	ret = clSetKernelArg(kernel0, 0, sizeof(cl_mem), (void *)&memobj_is_seed);
	ret = clSetKernelArg(kernel0, 1, sizeof(cl_mem), (void *)&memobj_visited);
	ret = clSetKernelArg(kernel0, 2, sizeof(cl_mem), (void *)&memobj_disconnect);
	
	ret = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void *)&memobj_r);
	ret = clSetKernelArg(kernel1, 1, sizeof(cl_mem), (void *)&memobj_p);
	ret = clSetKernelArg(kernel1, 2, sizeof(cl_mem), (void *)&memobj_max);
	
	ret = clSetKernelArg(kernel2, 0, sizeof(cl_mem), (void *)&memobj_p);
	ret = clSetKernelArg(kernel2, 1, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel2, 2, sizeof(cl_mem), (void *)&memobj_max);
	ret = clSetKernelArg(kernel2, 3, sizeof(cl_mem), (void *)&memobj_disconnect);
	ret = clSetKernelArg(kernel2, 4, sizeof(cl_mem), (void *)&memobj_is_seed);
	
	ret = clSetKernelArg(kernel3, 0, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel3, 1, sizeof(cl_mem), (void *)&memobj_disconnect);
	
	ret = clSetKernelArg(kernel5, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(kernel5, 1, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel5, 2, sizeof(cl_mem), (void *)&memobj_root_id);
	clFinish(command_queue);

	print_timestamp("Ejecutando primera fase de kerneles...\n", t);

	

	/* Encolar kerneles en cola de comandos. */
	ret = clEnqueueNDRangeKernel(command_queue, kernel0, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	printf("kernel0: %s\n", getErrorString(ret));

	ret = clEnqueueNDRangeKernel(command_queue, kernel1, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	printf("kernel1: %s\n", getErrorString(ret));

	ret = clEnqueueNDRangeKernel(command_queue, kernel2, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	printf("kernel2: %s\n", getErrorString(ret));
	
	ret = clEnqueueNDRangeKernel(command_queue, kernel3, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	printf("kernel3: %s\n", getErrorString(ret));
	
	clFinish(command_queue);

	/* Encolar regreso de datos. */

	map_is_seed = clEnqueueMapBuffer(command_queue, memobj_is_seed, CL_TRUE, CL_MAP_READ, 
																	0, sizeof(int) * tnumber, 0, NULL, NULL, &ret);

	printf("Read is_seed: %s\n", getErrorString(ret));

	map_area = clEnqueueMapBuffer(command_queue, memobj_area, CL_TRUE, CL_MAP_READ,
																	0, sizeof(double) * tnumber, 0, NULL, NULL,  &ret);

	map_root_id = clEnqueueMapBuffer(command_queue, memobj_root_id, CL_TRUE, CL_MAP_READ,
																	0, sizeof(int) * tnumber, 0, NULL, NULL,  &ret);
	map_adj = clEnqueueMapBuffer(command_queue, memobj_adj, CL_TRUE, CL_MAP_READ, 
																	0, 3 * sizeof(int) * tnumber, 0, NULL, NULL, &ret);

	clEnqueueUnmapMemObject(command_queue, memobj_adj, map_adj, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_area, map_area, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_root_id, map_root_id, 0, NULL, NULL);
	clFinish(command_queue);

	/* Forzar ejecución de comandos pendientes en cola. */
	clFinish(command_queue);
	#ifdef DEBUG
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
		if(adj[3 * e + 0] > -1)
			adj_mat[e][adj[3 * e + 0]] = 1;
		if(adj[3 * e + 1] > -1)
			adj_mat[e][adj[3 * e + 1]] = 1;
		if(adj[3 * e + 2] > -1)
			adj_mat[e][adj[3 * e + 2]] = 1;
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

	for(e = 0; e < tnumber;  e++){
		printf("%i ", is_seed[e]);
	}
	printf("\n");
	printf("\n");
	#endif

	print_timestamp("Ejecutando fase secuencial...\n", t);



	/* Etapa secuencial: Hacer un DFS a cada nodo no visitado, para comunicarle
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
	int i;
	
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
	
	clFinish(command_queue);
	print_timestamp("Ejecutando última fase de kerneles...\n", t);
	
	/* Encolar último kernel. */
	ret = clEnqueueNDRangeKernel(command_queue, kernel5, 1, NULL, global_work_size, localSize, 0, NULL, NULL);
	
	/* Encolar regreso de datos. */
	map_type = clEnqueueMapBuffer(command_queue, memobj_type, CL_TRUE, CL_MAP_WRITE, 
																	sizeof(type), 0, 0, NULL, NULL, NULL);
	map_area = clEnqueueMapBuffer(command_queue, memobj_area, CL_TRUE, CL_MAP_WRITE, 
																	sizeof(area), 0, 0, NULL, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_type, map_type, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_area, map_area, 0, NULL, NULL);
	
	/* Forzar ejecución de comandos pendientes en cola. */
	clFinish(command_queue);
	
	printf("* Número de vacíos internos: %d (%d triángulos)\n", num_ivoids, num_ivoid_triangs);
	printf("* Número de vacíos de borde: %d (%d triángulos)\n", num_bvoids, num_bvoid_triangs);
	printf("* Número de murallas: %d (%d triángulos)\n", num_walls, num_wall_triangs);
	printf("* Número de no-zonas: %d (%d triángulos)\n", num_nonzones, num_nonzone_triangs);
	
	print_timestamp("Escribiendo datos...\n", t);
	
	write_classification(cpath_prefix, root_id, type, area, tnumber, num_nonzone_triangs,
												num_ivoid_triangs, num_bvoid_triangs, num_wall_triangs);
	
	/* Liberar memoria. */
	free(r);
	free(p);
	free(adj);
	free(area);
	free(max);
	free(is_seed);
	free(root_id);
	free(visited);
	free(disconnect);
	free(type);
	
	print_timestamp("Fin.\n", t);
	
	return EXIT_SUCCESS;
}

