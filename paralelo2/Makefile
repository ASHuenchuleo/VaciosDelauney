#OCL_CFLAGS = -Wall -std=c89 -lm -ansi -I/usr/local/cuda-7.5/targets/x86_64-linux/include/ -L/usr/local/cuda-10.1/targets/x86_64-linux/lib/ -lOpenCL 

OCL_CFLAGS = -Wall -std=c89 -lm -ansi -lOpenCL 

ALL:
	gcc cpar.c ../shared/io.c ocl.c ../shared/adjgraph.c ../shared/timestamp.c ../shared/args.c  $(OCL_CFLAGS) -o uwu

DEBUG:
	gcc cpar.c ../shared/io.c ocl.c ../shared/adjgraph.c ../shared/timestamp.c ../shared/args.c  $(OCL_CFLAGS) -o uwu -DDEBUG

