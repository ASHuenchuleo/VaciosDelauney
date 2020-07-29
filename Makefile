###########################################################

## USER SPECIFIC DIRECTORIES ##

# CUDA directory:
CUDA_ROOT_DIR=/usr/local/cuda
PYTHON_DIR=/usr/include/python2.7

##########################################################

## CC COMPILER OPTIONS ##

# CC compiler options:
CC=g++
CC_FLAGS=-w
CC_LIBS=

##########################################################

## NVCC COMPILER OPTIONS ##

# NVCC compiler options:
NVCC=nvcc
NVCC_FLAGS=--compiler-bindir /usr/bin/gcc-8 
NVCC_LIBS=

# CUDA library directory:
CUDA_LIB_DIR= -L$(CUDA_ROOT_DIR)/lib64
# CUDA include directory:
CUDA_INC_DIR= -I$(CUDA_ROOT_DIR)/include
# CUDA linking libraries:
CUDA_LINK_LIBS= -lcudart

##########################################################

## Project file structure ##

# Source file directory:
SRC_DIR_CPU = life_seq/src
SRC_DIR_CUDA = life_cuda/src
SRC_DIR_OPENCL = life_opencl/src

# Object file directory:
OBJ_DIR = bin

# Include header file diretory:
INC_DIR = include


# python library directory:
PYTHON_INC_DIR= -I$(PYTHON_DIR)
# python include directory:
OPENCL_LINK_LIBS= -lpython2.7 -lOpenCL -lSDL2 

##########################################################

## Make variables ##

# Target executable name:
EXE = life

# Object files:
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/cuda_life.o $(OBJ_DIR)/opencl_life.o $(OBJ_DIR)/cpu_life.o  $(OBJ_DIR)/automata.o

##########################################################

## Compile ##

# Link c++ and CUDA compiled object files to target executable:
$(EXE) : $(OBJS)
	$(CC) $(OBJS) $(CC_FLAGS) -o $@  ${OPENCL_LINK_LIBS} $(CUDA_INC_DIR) $(CUDA_LIB_DIR) $(CUDA_LINK_LIBS)

# Compile main .cpp file to object files:
$(OBJ_DIR)/%.o : %.cpp
	$(CC)  -c $< $(CC_FLAGS) ${PYTHON_INC_DIR} ${OPENCL_LINK_LIBS} -o $@ 

# Compile C++ source files to object files:
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp include/%.h
	$(CC) -c $< $(CC_FLAGS)  -o $@

# Compile CUDA source files to object files:
$(OBJ_DIR)/%.o : $(SRC_DIR_CUDA)/%.cu $(INC_DIR)/%.cuh $(INC_DIR)/automata.h
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ $(NVCC_LIBS)

# Compile Opencl source files to object files:
$(OBJ_DIR)/%.o : $(SRC_DIR_OPENCL)/%.cpp $(INC_DIR)/%.h $(INC_DIR)/automata.h
	$(CC) ${CC_FLAGS} -c $< -o $@ ${CC_LIBS}

# Compile CPU source files to object files:
$(OBJ_DIR)/%.o : $(SRC_DIR_CPU)/%.cpp $(INC_DIR)/%.h $(INC_DIR)/automata.h
	$(CC) ${CC_FLAGS} -c $< -o $@ ${CC_LIBS}


# Clean objects in object directory.
clean:
	$(RM) bin/* *.o $(EXE) 
 
