
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>


#include <sched.h>

static struct timespec Calculate_dt(struct timespec first_time, struct timespec second_time){
  struct timespec time_delta;
  time_delta.tv_sec = second_time.tv_sec - first_time.tv_sec;
  if(first_time.tv_nsec > second_time.tv_nsec){
    time_delta.tv_nsec = 1000000000 + second_time.tv_nsec - first_time.tv_nsec;
    time_delta.tv_sec--;
  }
  else{
    time_delta.tv_nsec = second_time.tv_nsec - first_time.tv_nsec;
  }

  return time_delta;
}

int f1(int _n1, int _n2){

  /*truct timespec temp1,temp2;
  clock_gettime(CLOCK_MONOTONIC, &temp1);

  do{
    clock_gettime(CLOCK_MONOTONIC, &temp2);
  }while( Calculate_dt(temp1, temp2).tv_nsec/1000000 < 33);
  return _n1 + _n2;*/
  /*int k = _n1;
  for(int i = 0; i < 1000000*6; i++){
    k = k + _n2 + i;	
  }

  return k;*/

 struct timespec temp1,temp2, dt;
  for(int i= 0; i < 38; i++){
    clock_gettime(CLOCK_MONOTONIC, &temp1);
    do{
      clock_gettime(CLOCK_MONOTONIC, &temp2);
      dt = Calculate_dt(temp1,temp2);
    }while(dt.tv_nsec < 1000000);
  }

  return 0;
}


int f2(int _n1, int _n2){


    struct timespec temp1,temp2, dt;
     for(int i= 0; i < 58; i++){
       clock_gettime(CLOCK_MONOTONIC, &temp1);
       do{
         clock_gettime(CLOCK_MONOTONIC, &temp2);
         dt = Calculate_dt(temp1,temp2);
       }while(dt.tv_nsec < 1000000);
     }

     return 0;
}


int f3(int _n1, int _n2){

    struct timespec temp1,temp2, dt;
     for(int i= 0; i < 88; i++){
       clock_gettime(CLOCK_MONOTONIC, &temp1);
       do{
         clock_gettime(CLOCK_MONOTONIC, &temp2);
         dt = Calculate_dt(temp1,temp2);
       }while(dt.tv_nsec < 1000000);
     }


     return 0;

}
