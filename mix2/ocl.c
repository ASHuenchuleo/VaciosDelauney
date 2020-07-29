#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define MAX_SOURCE_SIZE 8192



cl_program create_program(cl_context context, char *source_path, cl_device_id device_id)
{
	FILE *fp;
	size_t source_size;
	char *source_str;
	cl_program program;
	cl_int ret;
	
	/* Load kernel source code */
	fp = fopen(source_path, "r");
	if (!fp)
	{
		fprintf(stderr, "** ERROR ** create_program: Programa \'%s\' no encontrado.\n", source_path);
		exit(EXIT_FAILURE);
	}
	
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	
	/* Create Kernel program from the read in source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	
	/* Build Kernel Program */
	char options[] = "";
	ret = clBuildProgram(program, 1, &device_id, options, NULL, NULL);
  printf("%d", ret);
  /*	if(ret != CL_SUCCESS)
	{
		fprintf(stderr, "** ERROR ** create_program: Programa \'%s\' no compiló.\n", source_path);
		exit(EXIT_FAILURE);
	}
	*/
  if (ret == CL_BUILD_PROGRAM_FAILURE) {
    size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    char *log = (char *) malloc(log_size);

    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    printf("%s\n", log);
}
	free(source_str);
	
	return program;
}





int upper_next_multiple(int n, int m)
{
	return m*ceil((double)n/(double)m);
}


int upper_next_multiple64(int n)
{
	int m = 64;
	return upper_next_multiple(n, m);
}
