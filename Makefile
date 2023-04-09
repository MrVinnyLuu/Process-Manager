# This Makefile is adapted from the Makefile provided in COMP20003

# define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
# define libraries to be linked
LIB = -lm

# define sets of header source files and object files
SRC = src/main.c src/record.c src/heap.c src/llist.c src/memory.c
SRC_PROCESS = src/process.c

# OBJ is the same as SRC, just replace .c with .o
OBJ = $(SRC:.c=.o)
OBJ_PROCESS = $(SRC_PROCESS:.c=.o)
 
# define the executable name(s)
EXE = allocate
EXE_PROCESS = process
 
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

$(EXE_PROCESS): $(OBJ_PROCESS)
	$(CC) $(CFLAGS) -o $(EXE_PROCESS) $(OBJ_PROCESS)

clean:
	rm -f $(OBJ) $(OBJ_PROCESS) $(EXE) $(EXE_PROCESS)