#include <stdio.h>
#include <time.h>
#include "func.h"


void f1(int, int);
void f2(int, int);
void f3(int, int);

struct timespec ini;
struct timespec end;
struct timespec result;

int timespec_subtract(struct timespec ts1, struct timespec ts2, struct timespec *result)
{
	if ( (ts1.tv_sec < ts2.tv_sec) || (ts1.tv_sec == ts2.tv_sec && (ts1.tv_nsec < ts2.tv_nsec) ) ) {
		return -1;
	}

	result->tv_sec = ts1.tv_sec - ts2.tv_sec;

	if (ts1.tv_nsec < ts2.tv_nsec) {
		ts1.tv_nsec += 1000000000L;
		(result->tv_sec)--;
	}
	result->tv_nsec = ts1.tv_nsec - ts2.tv_nsec;

	return 0;
}

int main(){
	clock_gettime(CLOCK_MONOTONIC, &ini);
	f1(1,2);
	clock_gettime(CLOCK_MONOTONIC, &end);
	
	timespec_subtract(ini, end, &result);
	printf("result=%ld\n",result.tv_sec);

return 0;
}
