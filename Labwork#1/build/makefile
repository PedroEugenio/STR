

# Configuration

# gcc for c or g++ for C++
CC = gcc
# flags to compilation
cflags = -g -Wall -O0 -D_REENTRANT -lpthread -pthread -lrt
# Directorie of the resource files #
RESOURCES = ../resources
# Directorie of the source files #
SOURCES = ../sources
# Directorie of the build files #
DEVEL = ../devel
# Header files
HEADERS  = ${RESOURCES}/func.h ${RESOURCES}/timespec.h
# Code files to compiling
FILES = ex1 ex2 ex3 ex4 ex5 test_time ex6 ex7 ex9


# call "make" -> Compile all OR make ex1 etc.
all: ${FILES}

# Remove all executables file
clean:
	rm -f ${FILES}


# Compilation of all problems

ex1:
	${CC} ${cflags} ${SOURCES}/ex1.c ${RESOURCES}/func.o -o ${DEVEL}/ex1 -lm

test_time: ${SOURCES}/test_time.c ${RESOURCES}/timespec.h
		${CC} ${cflags} ${SOURCES}/test_time.c -o ${DEVEL}/test_time

ex7: ${SOURCES}/ex7.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex7.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex7
