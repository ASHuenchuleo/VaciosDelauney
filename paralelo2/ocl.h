#include <CL/cl.h>

cl_program create_program(cl_context context, char *source_path, cl_device_id device_id);
int upper_next_multiple(int n, int m);
int upper_next_multiple64(int n);
