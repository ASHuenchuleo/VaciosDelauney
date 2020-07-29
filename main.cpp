 
#include <stdlib.h>
#include <chrono> 
#include <stdio.h>

#include "shared/matplotlibcpp.h"
namespace plt = matplotlibcpp;

struct device_test_results
{
	std::vector<double> nvals;
	std::vector<double> mevals_s;
};

device_test_results* test_dataset(int ntests, long int* sizearr, int niters, void (*fun (int, int, int, void*)),life_config* config){
    std::vector<double> nvals(ntests), mevals_s(ntests);

	fprintf(stdout, "\n");

	for(int i = 0; i < ntests; i++ )
	{	
		long int size = sizearr[i];
		int N = (int)sqrt(size);
		long int timesum = 0;
		fprintf(stdout, "N=%i\r", N);
		fflush(stdout);
		CpuAutomata * automata = NULL;
		test_result* result = (test_result*) fun(N, N, niters, config);
		automata = (CpuAutomata*) result->automata;
		long int time = result->time;
		fprintf(stdout, "N=%i, time %li nanoseconds\r", N, time);
		nvals.at(i) = size;
		mevals_s.at(i) = (double)size/time * 1000 * 1000 * 1000/1000000;
		delete automata;
	}
	fprintf(stdout, "\n");

	device_test_results* result = new device_test_results();
	result->nvals = nvals;
	result->mevals_s = mevals_s;
	return result;
}

void test(char* filename){


	life_config* config = new life_config();
	device_test_results* results_OPENCL;
	device_test_results* results_CUDA;
	config->animate = false;

	plt::figure_size(1200, 600);

	config->threadsPerBlock = tpb;
	config->local_mem = true; // Asegurarse de que BLOCK_SIZE en cuda sea consistente
	printf("CUDA\n");
	results_CUDA = test_dataset(ntests, sizearr, niters, run_cuda, config);
	printf("OpenCL\n");
	results_OPENCL = test_dataset(ntests, sizearr, niters, run_opencl, config);

	plt::named_semilogx("CUDA con memoria local", results_CUDA->nvals, results_CUDA->mevals_s, ".--");
	plt::named_semilogx("OpenCL con memoria local", results_OPENCL->nvals, results_OPENCL->mevals_s, ".--");

	config->threadsPerBlock = tpb;
	config->local_mem = false;
	printf("OpenCL\n");
	results_OPENCL = test_dataset(ntests, sizearr, niters, run_opencl, config);
	printf("CUDA\n");
	results_CUDA = test_dataset(ntests, sizearr, niters, run_cuda, config);

	plt::named_semilogx("CUDA sin memoria local", results_CUDA->nvals, results_CUDA->mevals_s, ".--");
	plt::named_semilogx("OpenCL sin memoria local", results_OPENCL->nvals, results_OPENCL->mevals_s, ".--");

	plt::title(title);
	plt::ylabel("millon de celdas/s ");
	plt::xlabel("celdas evaluadas");

	plt::legend();

	plt::save(savedir);
}

int main(int argc, char** argv){

	char* filename = "data.dat"

   	fprintf(stdout, "Test de memoria local..\n");
	test(filename);


    return 0;
} 
