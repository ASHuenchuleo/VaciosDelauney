 
#include <stdlib.h>
#include <stdio.h>

#include "shared/vacios.h"

long int test_dataset(int ntests, char* filename, double area_min, int (fun (int, char**))){
	fprintf(stdout, "\n");
	double timesum = 0;
	int i;
	for(i = 0; i < ntests; i++ )
	{	
		char buffer[50];
		sprintf(buffer, "%f", area_min); 
		char* exec_args[4] = {"uwu", filename, buffer, "data/classified/dat"};
		double time =  fun(4, exec_args);
		timesum += time;
	}
	fprintf(stdout, "\n");
	return timesum/ntests;
}

void test(int ntests, int npoints, double area_min, char* filename){


	double time_sec = 0;
	double time_par = 0;
	double time_mix = 0;
	double time = 0;

	printf("Secuencial\n");
	time_sec = test_dataset(ntests, filename, area_min, void_sec);
	printf("Mix\n");
	time_mix = test_dataset(ntests, filename, area_min, void_mix);
	printf("Paralelo\n");
	time_par = test_dataset(ntests, filename, area_min, void_par); 

	printf("ntris: %i,\n sequential: %f microsec,\n mix: %f microsec,\n parallel: %f microsec.\n", npoints, time_sec, time_mix, time_par);
}

int main(int argc, char** argv){

	int ntests = 1;

	char* filename;


	/*

	filename = "data/data.dat";
	test(ntests, 100, 50000000000, filename);

	filename = "data/30sphere2d_5000.dat";
	test(ntests, 5000, 500, filename);

	filename = "data/50sphere2d_15000.dat";
	test(ntests, 15000, 500, filename);

	filename = "data/30sphere2d_50000.dat";
	test(ntests, 50000, 500, filename);
	
	filename = "data/data_100000.dat";
	test(ntests, 1000000, 100, filename);

	filename = "data/data_300000.dat";
	test(ntests, 300000, 100, filename);

	filename = "data/data_600000.dat";
	test(ntests, 300000, 100, filename);
	
	filename = "data/data_1000000.dat";
	test(ntests, 1000000, 100, filename);

	filename = "data/data_3000000.dat";
	test(ntests, 3000000, 100, filename);

	filename = "data/data_6000000.dat";
	test(ntests, 6000000, 100, filename);

	filename = "data/data_10000000.dat";
	test(ntests, 1000000, 100, filename);

	filename = "data/data_20000000.dat";
	test(ntests, 2000000, 100, filename);

	filename = "data/data_40000000.dat";
	test(ntests, 4000000, 100, filename);

	filename = "data/data_60000000.dat";
	test(ntests, 6000000, 100, filename);
	*/
	filename = "data/data_20000000.dat";
	test(ntests, 2000000, 100, filename);

	filename = "data/data_40000000.dat";
	test(ntests, 4000000, 100, filename);

	filename = "data/data_60000000.dat";
	test(ntests, 6000000, 100, filename);
    return 0;
} 
