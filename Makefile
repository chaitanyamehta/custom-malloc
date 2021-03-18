CC = gcc
OPT = -O3
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .c files here (source files, excluding header files)
SRC = memory.c main.c

# List corresponding compiled object files here (.o files)
OBJ = memory.o main.o
 
#################################

# default rule

all: test
	@echo "my work is done here..."


# rule for making test

test: $(OBJ)
	$(CC) -o test $(CFLAGS) $(OBJ)
	@echo "-----------DONE -----------"


# generic rule for converting any .c file to any .o file
 
.c.o:
	$(CC) $(CFLAGS)  -c $*.c

# type "make clean" to remove all .o files plus the test binary

clean:
	rm -f *.o test

# type "make clobber" to remove all .o files (leaves test binary)

clobber:
	rm -f *.o

