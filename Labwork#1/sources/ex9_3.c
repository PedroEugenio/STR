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

#include "../resources/func.h"
#include "../resources/timespec.h"
#include "../resources/rtai.h"

/* Scheduler RMPO */
#define POLICY2USE SCHED_FIFO //SCHED_RR // SCHED_FIFO

/* Number of Threads */
#define TASKS      3

/* Activation periods in ms */
#define PRERIOD_T1 100
#define PRERIOD_T2 200
#define PRERIOD_T3 300

/* Start and Finish Time of threads in s */
#define START       3
#define FINISH      6  

#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Activation time for each thread */
struct timespec start_time; // [TASKS]
struct timespec response_time[TASKS] = {{0,0}, {0,0}, {0,0}};

void* task1(void *arg);
void* task2(void *arg);
void* task3(void *arg);

int main() {
  printf("\n\nProblem 9_3...\n\n");

  struct timespec current_time;

  /* Get the initial time */
  if( clock_gettime(CLOCK_MONOTONIC, &current_time) == -1 )
    errorExit("main->current_time");

  /* All tasks SART seconds after startup of de program */
  start_time = SUM(current_time, SET(START, 0));


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

    /* <----- Ex3 */
    priority[i].sched_priority -= i;
    printf("Priority of Task %d: %d\n", i+1, priority[i].sched_priority);

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
    /* Main thread go to sleep when wake up it will cancel tasks threads */
    sleep(FINISH);

    for (int i = 0; i < TASKS; i++) {
      pthread_cancel(thread[i]);
    }
    printf("TASK 1 - Response time: %LFs or %LFms\n", time2s(response_time[0]), time2ms(response_time[0]));
    printf("TASK 2 - Response time: %LFs or %LFms\n", time2s(response_time[1]), time2ms(response_time[1]));
    printf("TASK 3 - Response time: %LFs or %LFms\n", time2s(response_time[2]), time2ms(response_time[2]));

    /* Clear responses times */
    for (int i = 0; i < TASKS; i++) {
      response_time[i] = SET(0, 0);
    }
    exit(EXIT_SUCCESS);
  } while(1);
}
/*
*
* Thread for Task 1 or f1
*
*/
void* task1(void *arg) {

  struct timespec _time, last_activation;

  // Set Period and initialize next activation equal to start time
  // It will guarantee that in first interection they will start at the same time
  rt_task_make_periodic(pthread_self(), start_time, SET(0, PRERIOD_T1*MS2NS));

  // Executes until the main thread wake up and cancel this thread
  while(1){

    // Get the last next time of executation or the last activation time
    rt_task_get_next_period(&last_activation);
    // Sleep until the activation time and  set the new next activation time
    rt_task_wait_period();

    // Execute the task in her period
    f1(1,3);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    _time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(_time, response_time[0]))
      response_time[0] = _time;
  }
}
/*
*
* Thread for Task 2 or f2
*
*/
void* task2(void *arg) {

  struct timespec _time, last_activation;

  // Set Period and initialize next activation equal to start time
  // It will guarantee that in first interection they will start at the same time
  rt_task_make_periodic(pthread_self(), start_time, SET(0, PRERIOD_T2*MS2NS));

  // Executes until the main thread wake up and cancel this thread
  while(1){

    // Get the last next time of executation or the last activation time
    rt_task_get_next_period(&last_activation);
    // Sleep until the activation time and  set the new next activation time
    rt_task_wait_period();

    // Execute the task in her period
    f2(1,3);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    _time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(_time, response_time[1]))
      response_time[1] = _time;
  }

}
/*
*
* Thread for Task 2 or f2
*
*/
void* task3(void *arg) {

  struct timespec _time, last_activation;

  // Set Period and initialize next activation equal to start time
  // It will guarantee that in first interection they will start at the same time
  rt_task_make_periodic(pthread_self(), start_time, SET(0, PRERIOD_T3*MS2NS));

  // Executes until the main thread wake up and cancel this thread
  while(1){

    // Get the last next time of executation or the last activation time
    rt_task_get_next_period(&last_activation);
    // Sleep until the activation time and  set the new next activation time
    rt_task_wait_period();

    // Execute the task in her period
    f3(1,3);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    _time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(_time, response_time[2]))
      response_time[2] = _time;
  }
}
