

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
FILES = test_time Ex1 ex3 ex4 ex6 ex7 ex9_3 ex9_4


# call "make" -> Compile all OR make ex1 etc.
all: ${FILES}

# Remove all executables file
clean:
	rm -f ${FILES}


# Compilation of all problems

test_time: ${SOURCES}/test_time.c ${RESOURCES}/timespec.h
	${CC} ${cflags} ${SOURCES}/test_time.c -o ${DEVEL}/test_time

Ex1:
	${CC} ${cflags} ${SOURCES}/Ex1.c -o ${DEVEL}/Ex1 -lm


ex3: ${SOURCES}/ex3.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex3.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex3


ex4: ${SOURCES}/ex4.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex4.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex4


func2.o: ${RESOURCES}/func2.c
	${CC} -Wall -c ${RESOURCES}/func2.c


ex6_3: ${SOURCES}/ex3.c ${RESOURCES}/timespec.c ${RESOURCES}/timespec.h
	${CC} ${cflags} ${SOURCES}/ex3.c ../build/func2.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex6_3
	

ex6_4: ${SOURCES}/ex4.c ${RESOURCES}/timespec.c ${RESOURCES}/timespec.h
	${CC} ${cflags} ${SOURCES}/ex4.c ../build/func2.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex6_4


ex7: ${SOURCES}/ex7.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex7.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex7


ex9_3: ${SOURCES}/ex9_3.c ${RESOURCES}/rtai.h ${RESOURCES}/func.h ${RESOURCES}/timespec.c ${RESOURCES}/timespec.h
	${CC} ${cflags} ${SOURCES}/ex9_3.c ${RESOURCES}/rtai.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex9_3


problema9_3: ${SOURCES}/ex9_3.c ${RESOURCES}/func2.h ${RESOURCES}/func2.c ${RESOURCES}/timespec.h ${RESOURCES}/timespec.c ${RESOURCES}/rtai.h ${RESOURCES}/rtai.c
	gcc -g -Wall -O0 -D_REENTRANT -lpthread -pthread -lrt -DRTAI ${SOURCES}/ex9_3.c ${RESOURCES}/func2.c ${RESOURCES}/timespec.c ${RESOURCES}/rtai.c -o ${DEVEL}/problema9_3


ex9_4: ${SOURCES}/ex9_4.c ${RESOURCES}/timespec.c ${HEADERS}
	${CC} ${cflags} ${SOURCES}/ex9_4.c ${RESOURCES}/rtai.c ${RESOURCES}/func.o ${RESOURCES}/timespec.c -o ${DEVEL}/ex9_4


# ${HEADERS} # ${RESOURCES}/func2.h
