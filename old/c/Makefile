CC = gcc
CFLAGS = -Wall -std=c89  -ansi -lm
#OCL_CFLAGS = -Wall -std=c89 -lm -ansi -lOpenCL
OCL_CFLAGS = -Wall -std=c89  -ansi -I/usr/local/cuda-7.5/targets/x86_64-linux/include/ -L/usr/local/cuda-7.5/targets/x86_64-linux/lib/ -lOpenCL -lm

SRC_DIR = src
BIN_DIR = bin

CSEC_EXEC = $(BIN_DIR)/csec
TSVG_EXEC = $(BIN_DIR)/tsvg
CMIX_EXEC = $(BIN_DIR)/cmix
VCMP_EXEC = $(BIN_DIR)/vcmp

SHARED_DIR = $(SRC_DIR)/shared
CSEC_DIR = $(SRC_DIR)/csec
CMIX_DIR = $(SRC_DIR)/cmix
TSVG_DIR = $(SRC_DIR)/tsvg
VCMP_DIR = $(SRC_DIR)/vcmp

default: csec tsvg vcmp cmix

tsvg: tsvg_obj tsvg_tsvgio_obj tsvg_args_obj csec_triang_obj io_obj tsvg_aux_obj
	$(CC) -o $(TSVG_EXEC) tsvg.o tsvg_svgio.o io.o tsvg_args.o csec_triang.o tsvg_aux.o $(CFLAGS)

tsvg_tsvgio_obj: $(TSVG_DIR)/svgio.c
	$(CC) -c $(TSVG_DIR)/svgio.c -o tsvg_svgio.o $(CFLAGS)

tsvg_aux_obj: $(TSVG_DIR)/aux.c
	$(CC) -c $(TSVG_DIR)/aux.c -o tsvg_aux.o $(CFLAGS)

tsvg_args_obj: $(TSVG_DIR)/args.c
	$(CC) -c $(TSVG_DIR)/args.c -o tsvg_args.o $(CFLAGS)

tsvg_obj: $(TSVG_DIR)/tsvg.c
	$(CC) -c $(TSVG_DIR)/tsvg.c $(CFLAGS)

csec: csec_obj io_obj args_obj csec_triang_obj adjgraph_obj timestamp_obj
	$(CC) -o $(CSEC_EXEC) csec.o io.o args.o csec_triang.o adjgraph.o timestamp.o $(CFLAGS)

io_obj: $(SHARED_DIR)/io.c
	$(CC) -c $(SHARED_DIR)/io.c $(CFLAGS)

args_obj: $(SHARED_DIR)/args.c
	$(CC) -c $(SHARED_DIR)/args.c $(CFLAGS)

csec_obj: $(CSEC_DIR)/csec.c
	$(CC) -c $(CSEC_DIR)/csec.c $(CFLAGS)

csec_triang_obj: $(CSEC_DIR)/triang.c
	$(CC) -c $(CSEC_DIR)/triang.c -o csec_triang.o $(CFLAGS)

adjgraph_obj: $(SHARED_DIR)/adjgraph.c
	$(CC) -c $(SHARED_DIR)/adjgraph.c $(CFLAGS)

vcmp: vcmp_obj vcmp_tuple_obj vcmp_aux_obj vcmp_args_obj
	$(CC) -o $(VCMP_EXEC) vcmp.o vcmp_tuple.o vcmp_aux.o vcmp_args.o $(CFLAGS)

vcmp_obj: $(VCMP_DIR)/vcmp.c
	$(CC) -c $(VCMP_DIR)/vcmp.c $(CFLAGS)

vcmp_tuple_obj: $(VCMP_DIR)/tuple.c
	$(CC) -c $(VCMP_DIR)/tuple.c -o vcmp_tuple.o $(CFLAGS)

vcmp_aux_obj: $(VCMP_DIR)/aux.c
	$(CC) -c $(VCMP_DIR)/aux.c -o vcmp_aux.o $(CFLAGS)

vcmp_args_obj: $(VCMP_DIR)/args.c
	$(CC) -c $(VCMP_DIR)/args.c -o vcmp_args.o $(CFLAGS)

cmix: cmix_obj io_obj args_obj adjgraph_obj cmix_ocl_obj timestamp_obj
	$(CC) -o $(CMIX_EXEC) cmix.o io.o args.o adjgraph.o cmix_ocl.o timestamp.o $(OCL_CFLAGS)

cmix_obj: $(CMIX_DIR)/cmix.c
	$(CC) -c $(CMIX_DIR)/cmix.c $(OCL_CFLAGS)

cmix_ocl_obj: $(CMIX_DIR)/ocl.c
	$(CC) -c $(CMIX_DIR)/ocl.c -o cmix_ocl.o $(OCL_CFLAGS)

timestamp_obj: $(SHARED_DIR)/timestamp.c
	$(CC) -c $(SHARED_DIR)/timestamp.c -o timestamp.o $(CFLAGS)

clean:
	$(RM) $(BIN_DIR)/* *.o 
