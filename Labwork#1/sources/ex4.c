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


// Activation periods
#define PERIOD1_MS 100
#define PERIOD2_MS 200
#define PERIOD3_MS 300

#define NUMBER_THREADS 3

// Activation time for each thread
struct timespec activation_time[NUMBER_THREADS];

/*
  switch_time - time when to switch priority orders
  switch_dt - the time between priority switches
 */
struct timespec switch_time, switch_dt;

pthread_t thr[NUMBER_THREADS];

//  Variable to hold the worst response time
struct timespec worse_response_time[3] = { {0,0}, {0,0}, {0,0}};



// Thread that runs f1(). Switch priority orders from RMPO to inverse RMPO
void* function1(void* arg){

    // Wait for all tasks activate at the same time
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[0], NULL);


    // Create timespec for activation period
    struct timespec dt; dt.tv_sec = 0; dt.tv_nsec = (PERIOD1_MS)* 1000000;

    while(1){

        struct timespec temp;

        f1(2,3);

        // Execution time
        clock_gettime(CLOCK_MONOTONIC, &temp);
        struct timespec response_time = DIFF(temp, activation_time[0]);

        // Check if it's the worse response time so far
        struct timespec temp2 = DIFF(worse_response_time[0], response_time);
        if(temp2.tv_sec < 0 || temp2.tv_nsec < 0){
        worse_response_time[0] = response_time;
        }

        // Increment activation time
        activation_time[0] = SUM(activation_time[0], dt);


        // Get priority of this thread to know if running RMPO
        struct sched_param actual; int actual_int;
        pthread_getschedparam(pthread_self(),&actual_int, &actual);
        int priority = actual.sched_priority;

        // Check if switch time was reached
        temp = DIFF(switch_time, temp);
        if(priority == sched_get_priority_max(SCHED_FIFO) && (temp.tv_sec < 0 || temp.tv_nsec < 0) ){

        printf("Task 1 worse response time: %LFms\n", time2ms(worse_response_time[0]));
        printf("Task 2 worse response time: %LFms\n", time2ms(worse_response_time[1]));
        printf("Task 3 worse response time: %LFms\n", time2ms(worse_response_time[2]));

        worse_response_time[0].tv_sec = 0; worse_response_time[0].tv_nsec = 0;
        worse_response_time[1].tv_sec = 0; worse_response_time[1].tv_nsec = 0;
        worse_response_time[2].tv_sec = 0; worse_response_time[2].tv_nsec = 0;
        printf("======Switch to inverse RMPO======\n");

        // Time at which priority changed
        clock_gettime(CLOCK_MONOTONIC, &switch_time);

        // Set switch_time to current time + switch_dt
        switch_time = SUM(switch_time, switch_dt);

        // Switch task 1 and 3 priorities
        pthread_setschedprio(thr[2],sched_get_priority_max(SCHED_FIFO));
        pthread_setschedprio(thr[0],sched_get_priority_max(SCHED_FIFO)-2);

        }

        // Sleep until next activation time is reached
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[0], NULL);
    }

}


// Thread that runs f2().
void* function2(void* arg){
    // Wait for all tasks activate at the same time
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[1], NULL);

    // Create timespec for activation period
    struct timespec dt; dt.tv_sec = 0; dt.tv_nsec = (PERIOD2_MS)* 1000000;
    while(1){

        struct timespec temp;

        f2(2,3);

        // Execution time
        clock_gettime(CLOCK_MONOTONIC, &temp);
        struct timespec response_time = DIFF(temp, activation_time[1]);

        struct sched_param actual; int actual_int;
        pthread_getschedparam(thr[1],&actual_int, &actual);

        // Check if it's the worse response time so far
        struct timespec temp2 = DIFF(worse_response_time[1], response_time);
        if(temp2.tv_sec < 0 || temp2.tv_nsec < 0){
        worse_response_time[1] = response_time;
        }

        // Increment activation time
        activation_time[1] = SUM(activation_time[1], dt);

        // Sleep until next activation time is reached
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[1], NULL);
    }

}



// Thread that runs f3(). This thread switch the prioritys order from inverse RMPO to RMPO.
void* function3(void* arg){
    // Wait for all tasks activate at the same time
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[2], NULL);

    // Create timespec for activation period
    struct timespec dt; dt.tv_sec = 0; dt.tv_nsec = (PERIOD3_MS)* 1000000;
    while(1){

        struct timespec temp;

        f3(2,3);


        // Execution time
        clock_gettime(CLOCK_MONOTONIC, &temp);
        struct timespec response_time = DIFF(temp, activation_time[2]);

        struct timespec temp2 = DIFF(worse_response_time[2], response_time);
        if(temp2.tv_sec < 0 || temp2.tv_nsec < 0){
        worse_response_time[2] = response_time;
        }

        // Increment activation time
        activation_time[2] = SUM(activation_time[2], dt);



        // Get priority of the thread to know if it's running on inverse RMPO
        struct sched_param actual; int actual_int;
        pthread_getschedparam(pthread_self(),&actual_int, &actual);
        int priority = actual.sched_priority;

        // Check if switch time was reached
        temp = DIFF(switch_time, temp);
        if(priority == sched_get_priority_max(SCHED_FIFO) && (temp.tv_sec < 0 || temp.tv_nsec < 0) ){

        printf("Task 1 worse response time: %LFms\n", time2ms(worse_response_time[0]));
        printf("Task 2 worse response time: %LFms\n", time2ms(worse_response_time[1]));
        printf("Task 3 worse response time: %LFms\n", time2ms(worse_response_time[2]));

        worse_response_time[0].tv_sec = 0; worse_response_time[0].tv_nsec = 0;
        worse_response_time[1].tv_sec = 0; worse_response_time[1].tv_nsec = 0;
        worse_response_time[2].tv_sec = 0; worse_response_time[2].tv_nsec = 0;

        printf("======Switch to RMPO======\n");

        // Time at which priority changed
        clock_gettime(CLOCK_MONOTONIC, &switch_time);

        // Set switch_time to current time + switch_dt
        switch_time = SUM(switch_time, switch_dt);

        // Switch task 1 and 3 priorities
        pthread_setschedprio(thr[0],sched_get_priority_max(SCHED_FIFO));
        pthread_setschedprio(thr[2],sched_get_priority_max(SCHED_FIFO)-2);


        }
 
        // Sleep until next activation time is reached
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &activation_time[2], NULL);

    }
}


int main(){

    cpu_set_t my_set;
    pthread_attr_t attr[NUMBER_THREADS];
    struct sched_param priority[3];
    // Set all activation to 2 seconds after the current time
    struct timespec dt; dt.tv_sec = 2; dt.tv_nsec = 0;

    // Lock memory 
    mlockall(MCL_CURRENT|MCL_FUTURE);
  
    // Set threads affinity to CPU 0
    CPU_ZERO(&my_set);
    CPU_SET(0, &my_set);
  
    // Set the main thread to lowest priority without this the response times changed between switches
    pthread_t self = pthread_self();
    pthread_setschedprio(self, sched_get_priority_min(SCHED_FIFO));

    clock_gettime(CLOCK_MONOTONIC, &activation_time[0]);
    for(int i = 1; i < 3; i++){
        activation_time[i] = activation_time[0];
        activation_time[i].tv_sec += dt.tv_sec;
    }
    activation_time[0].tv_sec += dt.tv_sec;


    // For each thread change the scheduler to FIFO and set the priorities (in RMPO)
    
    // Set the parameters of threads
    pthread_attr_init(&attr[0]);
    pthread_attr_init(&attr[1]);
    pthread_attr_init(&attr[2]);

    // Set affinity of all threads, preventing biased times
    pthread_attr_setaffinity_np(&attr[0], sizeof(my_set), &my_set);
    pthread_attr_setaffinity_np(&attr[1], sizeof(my_set), &my_set);
    pthread_attr_setaffinity_np(&attr[2], sizeof(my_set), &my_set);

    // Set Explicit scheduling
    pthread_attr_setinheritsched(&attr[0], PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&attr[1], PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&attr[2], PTHREAD_EXPLICIT_SCHED);

    // Set Policy type to SCHED_FIFO
    pthread_attr_setschedpolicy(&attr[0], SCHED_FIFO);
    pthread_attr_setschedpolicy(&attr[1], SCHED_FIFO);
    pthread_attr_setschedpolicy(&attr[2], SCHED_FIFO);

    // Defines the priority of each thread
    priority[0].sched_priority = sched_get_priority_max(SCHED_FIFO);
    priority[1].sched_priority = sched_get_priority_max(SCHED_FIFO)-1;
    priority[2].sched_priority = sched_get_priority_max(SCHED_FIFO)-2;

    /* Sets the scheduling parameter attributes of the thread attributes object attr
    to the values specified in the buffer pointed to by priority */
    pthread_attr_setschedparam(&attr[0],&priority[0]);
    pthread_attr_setschedparam(&attr[1],&priority[1]);
    pthread_attr_setschedparam(&attr[2],&priority[2]);

    /* Set priority switch time */
    switch_dt.tv_sec = 5; switch_dt.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &switch_time);
    switch_time = SUM(switch_time, switch_dt);

    /* Create all the threads */
    printf("======Starting in RMPO======\n");
    if(pthread_create(&(thr[0]), &attr[0], function1, NULL) < 0){
        printf("Error %s\n",strerror(errno));
        exit(0);
    }
    if(pthread_create(&(thr[1]), &attr[1], function2, NULL) < 0){
        printf("Error %s\n",strerror(errno));
        exit(0);
    }
    if(pthread_create(&(thr[2]), &attr[2], function3, NULL) < 0){
        printf("Error %s\n",strerror(errno));
        exit(0);
    }


    /* Sleep for 35 seconds and then cancel all threads*/
    while(1){

        sleep(35);
        pthread_cancel(thr[0]);
        pthread_cancel(thr[1]);
        pthread_cancel(thr[2]);
        printf("Done!\n");
        exit(0);
        //pause();
    }


}