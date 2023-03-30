# This Makefile is adapted from the Makefile provided in COMP20003

# define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
# define libraries to be linked
LIB = -lm

# define sets of header source files and object files
SRC = src/main.c src/record.c src/heap.c src/queue.c

# OBJ is the same as SRC, just replace .c with .o
OBJ = $(SRC:.c=.o)
 
# define the executable name(s)
EXE = allocate

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

clean:
	rm -f $(OBJ) $(EXE)