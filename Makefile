###########################################################
## CC COMPILER OPTIONS ##

# CC compiler options:
CC=gcc
CC_FLAGS= -Wall -std=gnu11  -ansi -lm -lrt
CC_LIBS= -lOpenCL

##########################################################

## Project file structure ##

# Source file directory:
SRC_DIR_SEC = sec2
SRC_DIR_MIX = mix2
SRC_DIR_PAR = paralelo2

# Object file directory:
OBJ_DIR = bin

# Include header file diretory:
INC_DIR = shared


##########################################################

## Make variables ##

# Target executable name:
EXE = test

# Object files:
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/csec_test.o $(OBJ_DIR)/cpar_test.o $(OBJ_DIR)/cmix_test.o

##########################################################

## Compile ##
$(EXE) : $(OBJS) $(INC_DIR)/vacios.h ${INC_DIR}/io.c ${INC_DIR}/adjgraph.c ${INC_DIR}/timestamp.c ${INC_DIR}/args.c  $(SRC_DIR_SEC)/triang.c $(SRC_DIR_PAR)/ocl.c
	$(CC) $(OBJS) $(CC_FLAGS) ${CC_LIBS} -o $@  $(INC_DIR)/vacios.h ${INC_DIR}/io.c ${INC_DIR}/adjgraph.c ${INC_DIR}/timestamp.c ${INC_DIR}/args.c  $(SRC_DIR_SEC)/triang.c $(SRC_DIR_PAR)/ocl.c

# Compile aux .cpp files to object files:
$(OBJ_DIR)/%.o : $(INC_DIR)/%.c $(INC_DIR)/%.h
	$(CC)  -c $< $(CC_FLAGS) -o $@ 

# Compile main .c file to object files:
$(OBJ_DIR)/%.o : %.c $(INC_DIR)/vacios.h
	$(CC)  -c $< $(CC_FLAGS) -o $@ 

# Compile Versions
$(OBJ_DIR)/%.o : $(SRC_DIR_SEC)/%.c 
	$(CC) ${CC_FLAGS} -c $< -o $@ ${CC_LIBS}

$(OBJ_DIR)/%.o : $(SRC_DIR_PAR)/%.c 
	$(CC) ${CC_FLAGS} -c $< -o $@ ${CC_LIBS} 

$(OBJ_DIR)/%.o : $(SRC_DIR_MIX)/%.c 
	$(CC) ${CC_FLAGS} -c $< -o $@ ${CC_LIBS} 


# Clean objects in object directory.
clean:
	$(RM) bin/* *.o $(EXE) 
 
