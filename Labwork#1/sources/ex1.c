#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#include "../resources/func.h"

void f1(int, int);
void f2(int, int);
void f3(int, int);

struct timespec start;
struct timespec end;

struct timespec timespecDiff(struct timespec a, struct timespec b){
	struct timespec result;
	if ((a.tv_nsec - b.tv_nsec) < 0)
	{
		result.tv_sec = a.tv_sec - b.tv_sec - 1;
		result.tv_nsec = 1E9 + a.tv_nsec - b.tv_nsec;
	}
	else
	{
		result.tv_sec = a.tv_sec - b.tv_sec;
		result.tv_nsec = a.tv_nsec - b.tv_nsec;
	}
	return result;
}

long double timespecInS(struct timespec a){
	return (long double)(a.tv_sec + a.tv_nsec / 1E9);
}

int main(){

	cpu_set_t 	mask;

	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	if(sched_setaffinity(getpid(), sizeof(mask), &mask) == -1)
	{
		printf("Error on setaffinity!\n");
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	f1(1,2);
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Computation time of f1 = %LF s\n",timespecInS(timespecDiff(end, start)));
	clock_gettime(CLOCK_MONOTONIC, &start);
	f2(1,2);
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Computation time of f2 = %LF s\n",timespecInS(timespecDiff(end, start)));
	clock_gettime(CLOCK_MONOTONIC, &start);
	f3(1,2);
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Computation time of f3 = %LF s\n",timespecInS(timespecDiff(end, start)));

return 0;
}
