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

#define CLASS      1
#define GROUP      3

/* Activation periods in ms */
#define PRERIOD_T1 100
#define PRERIOD_T2 200
#define PRERIOD_T3 300

/* Start and Finish Time of threads in s */
#define START       2
#define FINISH      35
#define SWITCH      5

#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Activation time for each thread */
struct timespec start_time, switch_time; // [TASKS]
struct timespec response_time[TASKS] = {{0,0}, {0,0}, {0,0}};

/* For Exercise 9_4 we make global */
pthread_t thread[3];

void* task1(void *arg);
void* task2(void *arg);
void* task3(void *arg);
void response_times();

int main() {
  printf("\n\nProblem 9_4...\n\n");

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
  pthread_attr_t attr[3];
  struct sched_param priority[3];


  /* Set all threads affinity to CPU0 */
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  /* sets the affinity of the current process to that core */
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

    /* ----------> Ex3 <---------- */
    priority[i].sched_priority -= i;
    printf("Priority of Task %d: %d\n", i+1, priority[i].sched_priority);

    if( pthread_attr_setschedparam(&(attr[i]), &(priority[i])) != 0 )
      errorExit("main->pthread_attr_setschedparam");

  }/* end of for */

  /* ----------> Ex4 <---------- */
  /* Set the main thread to lowest priority */
  //pthread_t main_thread = pthread_self();
  pthread_setschedprio(pthread_self(), sched_get_priority_min(POLICY2USE));

  /* Initialize switch time */
  switch_time = SUM(switch_time, SET(SWITCH, 0));

  /* ----------> Ex4 <---------- */

  /* Create threads for all tasks. They will start in RMPO */
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
    printf("Inverse RMPO response times:\n");
    response_times();

    exit(EXIT_SUCCESS);
  } while(1);
}
/*
*
* Thread for Task 1 or f1
*
*/
void* task1(void *arg) {

  struct timespec _time, aux_time, last_activation;

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
    f1(CLASS, GROUP);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    aux_time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(aux_time, response_time[0]))
      response_time[0] = aux_time;

    /* --------> Exercise 4 <-------- */
    struct sched_param parameters;
    int policy, priority;

    // Get priority of the thread to know if is running RMPO
    pthread_getschedparam(pthread_self(), &policy, &parameters); // thread[0]

    priority = parameters.sched_priority;
    //_time = DIFF(switch_time, _time);
    //printf("%LF\n", time2ms(_time)); //
    // Check if switch time was reached
    if(priority == sched_get_priority_max(POLICY2USE) && IF_UPPER(_time, switch_time) ){ //IF_UPPER(_time, switch_time) (_time.tv_sec < 0 || _time.tv_nsec < 0)

      printf("RMPO response times:\n");
      response_times();

      // Switch to inverse RMPO
      clock_gettime(CLOCK_MONOTONIC, &_time);

      // Set switchTime = current_time + switch_delay
      switch_time = SUM(_time, SET(SWITCH, 0));

      // Switch Task 1 and 3 priorities
      pthread_setschedprio(thread[2], sched_get_priority_max(POLICY2USE));
      pthread_setschedprio(thread[0], sched_get_priority_max(POLICY2USE)-2);
    }
    /* --------> Exercise 4 <-------- */
  }/* end of while */
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
    f2(CLASS, GROUP);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    _time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(_time, response_time[1]))
      response_time[1] = _time;

  }/* end of while */
}
/*
*
* Thread for Task 2 or f2
*
*/
void* task3(void *arg) {

  struct timespec _time, aux_time, last_activation;

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
    f3(CLASS, GROUP);
    // Capture the actual time
    clock_gettime(CLOCK_MONOTONIC, &_time);
    // Response time = current_time - last_start_time
    aux_time = DIFF(_time, last_activation);// DIFF(next_activation, aux_time); -> valor positivo
    // Get the worse time:
    if(IF_UPPER(aux_time, response_time[2]))
      response_time[2] = aux_time;

    /* --------> Exercise 4 <-------- */
    struct sched_param parameters;
    int policy, priority;

    // Get priority of the thread to know if is running RMPO
    pthread_getschedparam(pthread_self(), &policy, &parameters); // thread[1]

    priority = parameters.sched_priority;
    //_time = DIFF(switch_time, _time);
    // Check if switch time was reached
    if(priority == sched_get_priority_max(POLICY2USE) && IF_UPPER(_time, switch_time)){

      printf("RMPO response times:\n");
      response_times();

      // Switch to inverse RMPO
      clock_gettime(CLOCK_MONOTONIC, &_time);

      // Set switchTime = current_time + switch_delay
      switch_time = SUM(_time, SET(SWITCH, 0));

      // Switch Task 1 and 2 priorities
      pthread_setschedprio(thread[0], sched_get_priority_max(POLICY2USE));
      pthread_setschedprio(thread[2], sched_get_priority_max(POLICY2USE)-2);
    }
    /* --------> Exercise 4 <-------- */
  }
}
void response_times() {
  /* Print response_times */
  printf("TASK 1: %LFs or %LFms\n", time2s(response_time[0]), time2ms(response_time[0]));
  printf("TASK 2: %LFs or %LFms\n", time2s(response_time[1]), time2ms(response_time[1]));
  printf("TASK 3: %LFs or %LFms\n", time2s(response_time[2]), time2ms(response_time[2]));

  /* Clear responses times */
  for (int i = 0; i < TASKS; i++) {
    response_time[i] = SET(0, 0);
  }
}
