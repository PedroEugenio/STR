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
#include <sys/types.h>
#include <sched.h>

#include "sources/func.h"
#include "sources/timespec.h"

/* Scheduler Round Robin */
#define POLICY2USE SCHED_RR

/* Number of Threads */
#define TASKS      3

/* Activation periods in ms */
#define PRERIOD_T1 100
#define PRERIOD_T2 200
#define PRERIOD_T3 300

#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Activation time for each thread */
struct timespec start_time[TASKS];
struct timespec worse_rptime[TASKS] = {{0,0}, {0,0}, {0,0}};

void* task1(void *arg);
void* task2(void *arg);
void* task3(void *arg);

int main() {
  printf("\n\nProblem 7...\n\n");

  struct timespec delay, current_time;

  delay.tv_sec = 3; // colocar função SET (do time)
  delay.tv_nsec = 0;


  /* Get the current time */
  if( clock_gettime(CLOCK_MONOTONIC, &current_time) == -1 )
    errorExit("main->current_time");

  /* All tasks start 3 seconds after startup of de program */
  for (int i = 0; i < TASKS; i++) {
    start_time[i] = SUM(current_time, delay);
  }

  /* Lock all of the calling process's virtual address space into RAM  Note: requires "sudo" to lock the memory. */
  if( mlockall(MCL_CURRENT | MCL_FUTURE) == -1 )
    errorExit("main->mloackall");


  cpu_set_t mask;
  pthread_t thread[3];
  pthread_attr_t attr[3];
  struct sched_param priority[3];


  /* Set all threads affinity to CPU0 */
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  /* sets the affinity of the current process to that core */ // O Luís não têm!
  if( sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1 )
    errorExit("main->sched_setaffinity");


  /* Initialize thread creation attributes */
  for (int i = 0; i < TASKS; i++) {


    /* Parameters of threads */
    if( pthread_attr_init(&(attr[i])) != 0 )
      errorExit("main->pthread_attr_init");


    /* Sets the affinity of all threads to the previously defined core */
    if( pthread_attr_setaffinity_np(&(attr[i]), sizeof(cpu_set_t), &mask) != 0 )
        errorExit("main->pthread_attr_setaffinity");


    /* By predefinition, the threads executes with implicit scheduling */
    if( pthread_attr_setinheritsched(&(attr[i]), PTHREAD_EXPLICIT_SCHED) != 0 )
      errorExit("main->pthread_attr_setinheritsched");

    /* Scheduler Round Robin */
    if( pthread_attr_setschedpolicy(&(attr[i]), POLICY2USE) != 0 )
      errorExit("main->pthread_attr_setschedpolicy");


    /* All threads with the same priority value */
    memset(&(priority[i]), 0, sizeof(struct sched_param));
    if( (priority[i].sched_priority = sched_get_priority_max(POLICY2USE)) == -1)
      errorExit("main->sched_get_priority_max");

    printf("Priority of %d: %d\n", i+1, priority[i].sched_priority);

    if( pthread_attr_setschedparam(&(attr[i]), &(priority[i])) != 0 )
      errorExit("main->pthread_attr_setschedparam");

  }/* end of for */


  /* Create threads for all tasks */
  if(pthread_create(&(thread[0]), &(attr[0]), task1, NULL) < 0)
    errorExit("main->pthread_create");

  if(pthread_create(&(thread[1]), &(attr[1]), task2, NULL) < 0)
    errorExit("main->pthread_create");

  if(pthread_create(&(thread[2]), &(attr[2]), task3, NULL) < 0)
    errorExit("main->pthread_create");


  /* Final results... for each task */
  do {
    printf("\n\nWaiting...\n\n");
    sleep(5); // ESTE TEMPO DEVE SER SUFICIENTE PARA CORRER O TEMPO DAS TASKS, SE FOR 0, NÃO SE CONSEGUE OBTER 
    for (int i = 0; i < TASKS; i++) {
      pthread_cancel(thread[i]);
    }
    printf("TASK 1 - Response time: %LFs or %LFms\n", time2s(worse_rptime[0]), time2ms(worse_rptime[0]));
    printf("TASK 2 - Response time: %LFs or %LFms\n", time2s(worse_rptime[1]), time2ms(worse_rptime[1]));
    printf("TASK 3 - Response time: %LFs or %LFms\n", time2s(worse_rptime[2]), time2ms(worse_rptime[2]));

    /* Clear worse responses times */
    for (int i = 0; i < TASKS; i++) {
      worse_rptime[i].tv_sec = 0; // usar a funçao SET
      worse_rptime[i].tv_nsec = 0;
    }
    exit(EXIT_SUCCESS);
  } while(1);
  // printf("Testing time: %LFms\n", time2ms(start_time[0]));
}
/*
*
* Thread for Task 1 or f1
*
*/
void* task1(void *arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[0], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T1 * MS2NS;

  while(1) {
    // Execute Task 1
    f1(1,3);
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[0]);
    // Worse time is:
    if(IF_UPPER(response_time, worse_rptime[0]))
      worse_rptime[0] = response_time;
    // Increment to next start time
    start_time[0] = SUM(t, start_time[0]);
    // Sleep until next activation time is reached
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[0], NULL);
  }
}
/*
*
* Thread for Task 2 or f2
*
*/
void* task2(void *arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[1], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T2 * MS2NS;

  while(1) {
    // Execute Task 2
    f1(1,3);
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[1]);
    // Worse time is:
    if(IF_UPPER(response_time, worse_rptime[1]))
      worse_rptime[1] = response_time;
    // Increment to next start time
    start_time[1] = SUM(t, start_time[1]);
    // Sleep until next activation time is reached
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[1], NULL);
  }
}
/*
*
* Thread for Task 2 or f2
*
*/
void* task3(void *arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[2], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T3 * MS2NS;

  while(1) {
    // Execute Task 3
    f1(1,3);
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[2]);
    // Worse time is:
    if(IF_UPPER(response_time, worse_rptime[2]))
      worse_rptime[2] = response_time;
    // Increment to next start time
    start_time[2] = SUM(t, start_time[2]);
    // Sleep until next activation time is reached
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[2], NULL);
  }
}
