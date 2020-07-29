/* Programa principal para O(n) mixto. */

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

#define KERNELS_FILE_PATH "../src/cmix/kernels.cl.c"



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
	cl_mem memobj_adj;
	cl_mem memobj_max;
	cl_mem memobj_is_seed;
	cl_mem memobj_root_id;
	cl_mem memobj_area;
	cl_mem memobj_visited;
	cl_mem memobj_disconnect;
	cl_mem memobj_type;
	cl_program program;
	cl_kernel kernel0;
	cl_kernel kernel1;
	cl_kernel kernel2;
	cl_kernel kernel3;
	cl_kernel kernel4;
	cl_kernel kernel5;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	char dev_name[1024];
	void *map_adj;
	void *map_is_seed;
	void *map_type;
	void *map_area;
	
	/* Obtener información de plataforma/dispositivo. */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(dev_name), dev_name, NULL);
	
	printf("* Trabajando con: %s\n", dev_name);
	
	/* Crear contexto de OpenCL. */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	
	/* Crear cola de comandos. */
	/*command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);*/
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	
	/* Crear búferes de memoria. */
	memobj_r = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(r), r, &ret);
	memobj_p = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(p), p, &ret);
	memobj_adj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(adj), adj, &ret);
	memobj_max = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(max), max, &ret);
	memobj_is_seed = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(is_seed), is_seed, &ret);
	memobj_root_id = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(root_id), root_id, &ret);
	memobj_area = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(area), area, &ret);
	memobj_visited = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(visited), visited, &ret);
	memobj_disconnect = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(disconnect), disconnect, &ret);
	memobj_type = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(type), type, &ret);
	
	/* Crear programa. */
	program = create_program(context, KERNELS_FILE_PATH, device_id);
	
	/* Crear kerneles. */
	kernel0 = clCreateKernel(program, "initialize_values", &ret);
	kernel1 = clCreateKernel(program, "mark_max_max", &ret);
	kernel2 = clCreateKernel(program, "mark_disconnections", &ret);
	kernel3 = clCreateKernel(program, "disconnect", &ret);
	kernel4 = clCreateKernel(program, "mark_seeds", &ret);
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
	
	ret = clSetKernelArg(kernel3, 0, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel3, 1, sizeof(cl_mem), (void *)&memobj_disconnect);
	
	ret = clSetKernelArg(kernel4, 0, sizeof(cl_mem), (void *)&memobj_p);
	ret = clSetKernelArg(kernel4, 1, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel4, 2, sizeof(cl_mem), (void *)&memobj_max);
	ret = clSetKernelArg(kernel4, 3, sizeof(cl_mem), (void *)&memobj_is_seed);
	
	ret = clSetKernelArg(kernel5, 0, sizeof(cl_mem), (void *)&memobj_type);
	ret = clSetKernelArg(kernel5, 1, sizeof(cl_mem), (void *)&memobj_adj);
	ret = clSetKernelArg(kernel5, 2, sizeof(cl_mem), (void *)&memobj_root_id);
	
	/* Trabajar en una dimensión. */
	size_t global_work_size[3] = {tnumber, 0, 0}; /* <--- POR CHEQUEAR SI SUPERA MAXIMO */
	clFinish(command_queue);
	print_timestamp("Ejecutando primera fase de kerneles...\n", t);
	
	/* Encolar kerneles en cola de comandos. */
	ret = clEnqueueNDRangeKernel(command_queue, kernel0, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(command_queue, kernel1, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(command_queue, kernel2, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(command_queue, kernel3, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(command_queue, kernel4, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	
	/* Encolar regreso de datos. */
	map_adj = clEnqueueMapBuffer(command_queue, memobj_adj, CL_TRUE, CL_MAP_WRITE, 
																	sizeof(adj), 0, 0, NULL, NULL, NULL);
	map_is_seed = clEnqueueMapBuffer(command_queue, memobj_is_seed, CL_TRUE, CL_MAP_WRITE, 
																	sizeof(is_seed), 0, 0, NULL, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_adj, map_adj, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, memobj_is_seed, map_is_seed, 0, NULL, NULL);
	
	/* Forzar ejecución de comandos pendientes en cola. */
	clFinish(command_queue);
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
	ret = clEnqueueNDRangeKernel(command_queue, kernel5, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	
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
