

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
HEADERS  = ${RESOURCES}/timespec.h
# Code files to compiling
FILES = test_time Ex1


# call "make" -> Compile all OR make ex1 etc.
all: ${FILES}

# Remove all executables file
clean:
	rm -f ${FILES}


# Compilation of all problems

test_time: ${SOURCES}/test_time.c ${RESOURCES}/timespec.h
	${CC} ${cflags} ${SOURCES}/test_time.c -o ${DEVEL}/test_time

Ex1:
	${CC} ${cflags} ${SOURCES}/Ex1.c ${RESOURCES}/timespec.c -o ${DEVEL}/Ex1 -lm


ex3: ${SOURCES}/ex3.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex3.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex3


# ${HEADERS} # ${RESOURCES}/func2.h
