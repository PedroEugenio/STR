

# Configuration

# gcc for c or g++ for C++
CC = gcc
# flags to compilation
cflags = -g -Wall -O0 -D_REENTRANT -lpthread -pthread -lrt
# Header files
HEADERS = timespec.h
# Code files to compiling
FILES = test_time


# call "make" -> Compile all OR make ex1 etc.
all: ${FILES}

# Remove all executables file
clean:
	rm -f ${FILES}


# Compilation of all problem # timespec.c 

test_time: test_time.c ${HEADERS}
	${CC} ${cflags} test_time.c -o test_time
