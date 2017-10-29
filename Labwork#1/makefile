

# Configuration

# gcc for c or g++ for C++
CC = gcc
# flags to compilation
cflags = -g -Wall -O0 -D_REENTRANT -lpthread -pthread -lrt
# Directorie of the source files #
DIR = sources
# Header files
HEADERS  = ${DIR}/func.h ${DIR}/timespec.h
# Code files to compiling
FILES = ex1 ex2 ex3 ex4 ex5 ex6 ex7 ex8 ex9


# call "make" -> Compile all OR make ex1 etc.
all: ${FILES}

# Remove all executables file
clean:
	rm -f ${FILES}


# Compilation of all problems

ex1:
	${CC} ${cflags} ex1.c ${DIR}/func.o -o ex1 -lm

ex7: ex7.c ${DIR}/timespec.c ${HEADERS}
	${CC} ${cflags} ex7.c ${DIR}/func.o ${DIR}/timespec.c -o ex7
