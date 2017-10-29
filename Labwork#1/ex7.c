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

#include "sources/fun.h"
#include "sources/timespec.c"

/* Number of Threads */
#define TASKS      3

/* Activation periods in ms */
#define PRERIOD_T1 100
#define PRERIOD_T2 200
#define PRERIOD_T3 300

/* Activation time for each thread */
struct timespec start_time[3];
struct timespec worse_rptime[3];

void* task1(void* arg);
void* task2(void* arg);
void* task3(void* arg);


int main() {
  printf("Problem 7...\n\n");

  struct timespec delay, curent_time;

  delay.tv_sec = 3;
  delay.t_nsec = 0;

  /* Get the current time */
  clock_gettime(CLOCK_MONOTONIC, &curent_time);

  /* All tasks start 3 seconds after startup of de program */
  for (int i = 0; i < TASKS; i++) {
    start_time[i] = SUM(current_time, delay);
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  pthread_t thread[3];
  pthread_attr_t attr[3];
  struct sched_param priority[3];

  /* Initialize thread creation attributes */
  for (int i = 0; i < TASKS; i++) {
    pthread_attr_init(&attr[i]);
    pthread_attr_setschedpolicy(&attr[i], SCHED_RR);
    /* All threads with the same priority value */
    priority[i].sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr[i], &priority[i]);
    /* Scheduler Round Robin */
    pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED);
  }


  /* Set all threads affinity to CPU0 */
  cpu_set_t PC;
  CPU_ZERO(&PC);
  CPU_SET(0, &PC);

  for (int i = 0; i < TASKS; i++) {
    pthread_attr_setaffinity_np(&thread[i], &PC);
  }


  /* Create threads for all tasks */
  if(pthread_create(&thread[0], &attr[0], task1, NULL) < 0)
    printf("Error %s\n", strerror(errno));
  if(pthread_create(&thread[1], &attr[1], task2, NULL) < 0)
    printf("Error %s\n", strerror(errno));
  if(pthread_create(&thread[2], &attr[2], task3, NULL) < 0)
    printf("Error %s\n", strerror(errno));

  /* Final results... for each sample */
  do {
    printf("Sleeping...\n\n");
    sleep(5);
    for (int i = 0; i < TASKS; i++) {
      pthread_cancel(thread[i]);
    }
    printf("1 - worse response time: %lds and %ldms\n", time2s(worse_rptime[0]), time2ms(worse_rptime[0]));
    printf("2 - worse response time: %lds and %ldms\n", time2s(worse_rptime[1]), time2ms(worse_rptime[1]));
    printf("3 - worse response time: %lds and %ldms\n", time2s(worse_rptime[2]), time2ms(worse_rptime[2]));

    /* Clear worse responses times */
    for (int i = 0; i < TASKS; i++) {
      worse_rptime[i].tv_sec = 0;
      worse_rptime[i].tv_nsec = 0;
    }
    //return 0;
  } while(1);
}
/*
*
* Thread for Task 1 or f1
*
*/
void* task1(void* arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[0], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T1 * M2NS;

  while(1) {
    // Execute Task 1
    f1(1,2);
    // Log execution end time
    //struct timespec response_time;
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[0]);
    // Worse time is:
    worse_rptime[0] = IF_UPPER(worse_rptime[0], response_time);
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
void* task2(void* arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[1], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T2 * M2NS;

  while(1) {
    // Execute Task 1
    f1(1,2);
    // Log execution end time
    //struct timespec response_time;
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[1]);
    // Worse time is:
    worse_rptime[1] = IF_UPPER(worse_rptime[1], response_time);
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
void* task3(void* arg) {

  // All tasks must be activated at the same time
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[3], NULL);

  struct timespec t, response_time;

  // Initialize the activation period of Task 1
  t.tv_sec = 0;
  t.tv_nsec = PRERIOD_T3 * M2NS;

  while(1) {
    // Execute Task 1
    f1(1,2);
    // Log execution end time
    //struct timespec response_time;
    // Capture the end time (a)
    clock_gettime(CLOCK_MONOTONIC, &response_time);
    // Response time = end_time - last_start_time
    response_time = DIFF(response_time, start_time[3]);
    // Worse time is:
    worse_rptime[3] = IF_UPPER(worse_rptime[3], response_time);
    // Increment to next start time
    start_time[3] = SUM(t, start_time[3]);
    // Sleep until next activation time is reached
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time[3], NULL);
  }
}
