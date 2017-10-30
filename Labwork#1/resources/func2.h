
#ifndef FUNC2H
#define FUNC2H

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>


#ifndef threadErrorExit
#define threadErrorExit(msg)

  do {
    perror(mg);
    pthread_exit(NULL);
  } while(0);

#endif

#define TIME1         38000000 	// ns
#define TIME2 			  58000000 	// ns
#define TIME3 			  88000000 	// ns
#define WAIT_TIME    	1000000 	// ns

void f1(int, int);
void f2(int, int);
void f3(int, int);

void wait_time();

#endif
