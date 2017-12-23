/************************************************************************
 *
 *  Labwork #2 - Real Time Systems
 *
 *  Group: Frederico Vaz & Pedro Eugénio
 *
 *  Part II
 *
 ************************************************************************/
#define _GNU_SOURCE

#include"ros/ros.h"
#include"std_msgs/String.h" // To make cout or ROS_INFO
#include<sensor_msgs/PointCloud.h>
#include<sensor_msgs/PointCloud2.h>
#include<sensor_msgs/point_cloud_conversion.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // int_8 int_32 etc
#include <time.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h> // ??
#include <sys/types.h>
#include <sched.h>
#include <semaphore.h>
#include "timespec.h" // pointCloud
#include "timespec.c" // pointCloud

using namespace std;

/* MAX number of Points that our program can read */
#define NUM_POINTS   20000
/* Scheduler RMPO */
#define POLICY2USE SCHED_FIFO //SCHED_RR // SCHED_FIFO
/* Number of Tasks/Threads to do it */
#define TASKS        3

/* Struct to store coordinates from Point Clouds files */
struct PointCloud{

  float x[NUM_POINTS];
  float y[NUM_POINTS];
  float z[NUM_POINTS];

  float average[3];
  float min[3];
  float max[3];
  float std[3]; //standard deviation

  int count_filtered_points;
  int count_points;

};

/* One threads for each task */
pthread_t thread[TASKS];

/* Total Computation time for each CRITICAL TASK CODE ZONE */
struct timespec critical_response_time[TASKS] = {{0,0}, {0,0}, {0,0}};

/* Total Computation time for each task */
struct timespec total_response_time[TASKS] = {{0,0}, {0,0}, {0,0}};

/* Create a Global Semaphore */
sem_t mutex;

/* Error Message */
#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Global functions declaration */
void task1(struct PointCloud *temp); // void* void* task1(void *arg);
void task2(struct PointCloud *temp);
void task3(struct PointCloud *temp);

void execute_math(struct PointCloud *temp);
void response_times();

void read_file(struct PointCloud *temp);
void write_file(struct PointCloud *temp);

void calc_average(struct PointCloud *temp);
void calc_min(struct PointCloud *temp);
void calc_max(struct PointCloud *temp);
void calc_std(struct PointCloud *temp);

void x_negative_filter(struct PointCloud *temp);
void denoise(struct PointCloud *temp);
void ramps(struct PointCloud *temp);

void pointcloudCallback(sensor_msgs::PointCloud2::ConstPtr scan_out);
void process();

/* Create a ROS Publisher to public a output point cloud message */
ros::Publisher newPointCloud;

/* Create a global message type PointCloud2 */
sensor_msgs::PointCloud2::ConstPtr pointcloud;

sensor_msgs::PointCloud input;
sensor_msgs::PointCloud output;

/* To run our threads  */
bool runflag = false;

/*
                                                                              */
int main(int argc, char **argv) {

  /* Initialize ROS node  */
  ros::init(argc, argv, "pointCloud");

  /* Create a handle to this process node */
  ros::NodeHandle nh;

  /* Create a ROS Subscriber to subscriber a intput point cloud message */
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2>("/velodyne_points", 100, pointcloudCallback);

  /* It's Global in our case  - Create a ROS Publisher to public a output point cloud message */
  newPointCloud = nh.advertise<sensor_msgs::PointCloud>("/output_results", 100);

  /* Specify a frequency that you would like to loop at */
  ros::Rate rate(10);

  struct PointCloud pointCloud; // original file

  /* Reset counter to number of points readed in original files */
  pointCloud.count_points = 0;
  /* Reset counter to number of points writed in filtered files */
  pointCloud.count_filtered_points = 0;

  while(nh.ok()) {
    /*
    ros::ok() will return false if:
    - a SIGINT is received (Ctrl-C)
    - we have been kicked off the network by another node with the same name
    - ros::shutdown() has been called by another part of the application.
    - all ros::NodeHandles have been destroyed
    */

    if(runflag) {

      //process();

      //task1(&pointCloud);
      //task2(&pointCloud);
      //task3(&pointCloud);
      for(int i = 0; i < input.points.size(); i++){
        // Saves the values from .txt file to the variables
        output.points[i].x = input.points[i].x; // temp->x[temp->count_points]
        output.points[i].y = input.points[i].y; // temp->y[temp->count_points]
        output.points[i].z = input.points[i].z; // temp->z[temp->count_points]

        //temp->count_points++;  // last line of coord is all 0
      }

      runflag = false;
    }

    /* if you were to add a subscription into this application, and did not have
    ros::spinOnce() here, your callbacks would never get called
    - Spin, waiting for messages to arrive
    - When a message arrives, the chatterCallback() function is called */
    ros::spinOnce();
    rate.sleep();
  }
  return 1;
}
/*******************************************************************************
*
* Objective: Callback Subscriber
*
* Issues:
*
*******************************************************************************/
void pointcloudCallback(sensor_msgs::PointCloud2::ConstPtr scan_out) {

  //pointcloud = scan_out;

  //sensor_msgs::PointCloud output;

  sensor_msgs::convertPointCloud2ToPointCloud(*scan_out, input); // *pointcloud, input

  //std::cout << "Points: " << scan_out->height*scan_out->width << std::endl;

  std::cout << "Points: " << input.points.size() << std::endl;

  //for (int i = 0; i < output.points.size(); i++) {
    //std::cout << "X: " << output.points[i].x << "Y: " << output.points[i].y << "Z: " << output.points[i].z << std::endl;
  //}

  runflag = true;
}
/*******************************************************************************
*
* Objective:  Read the values from the file and pass the values to arrays/matrix
*             inside a struct variable.
*             Calculate and print: the number of points, minimum, maximum
*             average, stand-deviation.
*
* Issues:
*
*******************************************************************************/
void task1(struct PointCloud *temp) {

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  printf("\n---------------------------- TASK1 -----------------------------\n");

  struct timespec start_time, critical_end_time, total_end_time;

  /* Capture the start time */
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  /*                    BEGIN OF CRITICAL CODE ZONE                            *
   * we want to acess a unique resource: the Point Cloud                       */
  //sem_wait(&mutex);

  /* Read orginal PointCloud/file */
  read_file(temp);

  /* Do the math on PointCloud */
  calc_average(temp);
  calc_min(temp);
  calc_max(temp);
  calc_std(temp);

  printf("Number of points: %i\n", temp->count_points);
  printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", temp->average[0], temp->average[1],temp->average[2]);
  printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", temp->min[0], temp->min[1], temp->min[2]);
  printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", temp->max[0], temp->max[1], temp->max[2]);
  printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", temp->std[0], temp->std[1], temp->std[2]);

  /*                      END OF CRITICAL CODE ZONE                            *
   * we don't need anymore to acess a unique resource: the Point Cloud         */
  //sem_post(&mutex);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &critical_end_time);

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  /* Response time = current_time - last_start_time */
  critical_response_time[0] = DIFF(critical_end_time, start_time);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &total_end_time);
  total_response_time[0] = DIFF(total_end_time, start_time);
}
/*******************************************************************************
*
* Objective: Remove all the points that are located in the “back/behind” part
*            of the car with respect to the sensor (ie, negative values of x.)
*            Detect and remove two groups (clusters) of points that are located
*            very close to the car. Discard those points that clearly do not
*            correspond to the ground/road (ie, the Outliers)
*
* Issues:   Not implemented points that clearly do not correspond to ground
*
*******************************************************************************/
void task2(struct PointCloud *temp) {

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  printf("\n---------------------------- TASK2 -----------------------------\n");

  struct timespec start_time, critical_end_time, total_end_time;

  /* Capture the start time */
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  /*                    BEGIN OF CRITICAL CODE ZONE                            *
   * we want to acess a unique resource: the Point Cloud                       */
  //sem_wait(&mutex);

  /* Removing axis X negative Points and Clusters were removed above too       */
  x_negative_filter(temp);

  //execute_math(count_filtered_points);
  // It's necessary update the math after filtering?
  // Do it in the end, if we can achieve better results

  /*                      END OF CRITICAL CODE ZONE                            *
   * we don't need anymore to acess a unique resource: the Point Cloud         */
  //sem_post(&mutex);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &critical_end_time);

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  /* Response time = current_time - last_start_time */
  critical_response_time[1] = DIFF(critical_end_time, start_time);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &total_end_time);
  total_response_time[1] = DIFF(total_end_time, start_time);
}
/*******************************************************************************
*
* Objective: Detect the points that belong to the drivable area with respect to
*            the car i.e LIDAR points that belong to the ground/road
*
* Issues: Not implemented. I put here 2.3) discard the Outliers
*
*******************************************************************************/
void task3(struct PointCloud *temp) {

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  printf("\n---------------------------- TASK3 -----------------------------\n");

  struct timespec start_time, critical_end_time, total_end_time;

  /* Capture the start time */
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  /*                    BEGIN OF CRITICAL CODE ZONE                            *
   * we want to acess a unique resource: the Point Cloud                       */
  //sem_wait(&mutex);

  /* STD filtering - Removing Noising Points */
  denoise(temp);

  //execute_math(count_filtered_points);
  // It's necessary update the math after filtering?
  // Do it in the end, if we can achieve better results

  //write_file(temp);

  /*                      END OF CRITICAL CODE ZONE                            *
   * we don't need anymore to acess a unique resource: the Point Cloud         */
  //sem_post(&mutex);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &critical_end_time);

  /* Do some no critical things i.e some resource that                         *
   * threads don't need to acess at the same time                              */

  // Response time = current_time - last_start_time
  critical_response_time[2] = DIFF(critical_end_time, start_time);

  /* Capture the end time */
  clock_gettime(CLOCK_MONOTONIC, &total_end_time);
  total_response_time[2] = DIFF(total_end_time, start_time);
}
/*******************************************************************************
*
* Objective: Math tasks to do in each Point Cloud
* Issues:
*
*******************************************************************************/
void execute_math(struct PointCloud *temp) {

  calc_average(temp);
  calc_min(temp);
  calc_max(temp);
  calc_std(temp);

}
/*******************************************************************************
*
* Objective: Computational time for each Task
*
* Issues: Melhorar esta função, e fazer dela uma só para impressão de tempos
*
*******************************************************************************/
void response_times() {

  struct timespec total_time;

  printf("\n-------------- TIME ON CRITICAL TASKS CODE ZONE ----------------\n");

  /* Print response_times */
  printf("TASK 1: %LFs or %LFms\n", time2s(critical_response_time[0]), time2ms(critical_response_time[0]));
  printf("TASK 2: %LFs or %LFms\n", time2s(critical_response_time[1]), time2ms(critical_response_time[1]));
  printf("TASK 3: %LFs or %LFms\n", time2s(critical_response_time[2]), time2ms(critical_response_time[2]));

  total_time = SUM(critical_response_time[1], critical_response_time[0]);
  total_time = SUM(critical_response_time[2], total_time);

  printf("TOTAL TIME: %LFs or %LFms\n", time2s(total_time), time2ms(total_time));

  /* Clear responses times */
  for (int i = 0; i < TASKS; i++) {
    critical_response_time[i] = SET(0, 0);
  }

  printf("\n-------------------- TOTAL TIME OF TASKS -----------------------\n");

  /* Print response_times */
  printf("TASK 1: %LFs or %LFms\n", time2s(total_response_time[0]), time2ms(total_response_time[0]));
  printf("TASK 2: %LFs or %LFms\n", time2s(total_response_time[1]), time2ms(total_response_time[1]));
  printf("TASK 3: %LFs or %LFms\n", time2s(total_response_time[2]), time2ms(total_response_time[2]));

  total_time = SUM(total_response_time[1], total_response_time[0]);
  total_time = SUM(total_response_time[2], total_time);

  printf("TOTAL TIME: %LFs or %LFms\n", time2s(total_time), time2ms(total_time));

  /* Clear responses times */
  for (int i = 0; i < TASKS; i++) {
    total_response_time[i] = SET(0, 0);
  }
}
/*******************************************************************************
*
* Objective: Read file from directory and save data into a structure array
* Notes: int file - number of the file required
*
*******************************************************************************/
void read_file(struct PointCloud *temp){


  for(int i = 0; i < input.points.size(); i++){
    // Saves the values from .txt file to the variables
    output.points[i].x = input.points[i].x; // temp->x[temp->count_points]
    output.points[i].y = input.points[i].y; // temp->y[temp->count_points]
    output.points[i].z = input.points[i].z; // temp->z[temp->count_points]

    temp->count_points++;  // last line of coord is all 0
  }

}
/*******************************************************************************
*
* Objective: Save filtered data into a new file
* Notes:
* int file - number of the file
*******************************************************************************/
void write_file(struct PointCloud *temp){

  //set message header (using the same from /velodyne_points)
  output.header = input.header;

  for(int i = 0; i < temp->count_filtered_points; i++){
    // Saves the values from .txt file to the variables
    output.points[i].x = temp->x[temp->count_points];
    output.points[i].y = temp->y[temp->count_points];
    output.points[i].z = temp->z[temp->count_points];
  }
  /* Publish/broadcast the message to anyone who is connected */
  newPointCloud.publish(output);
}
/*******************************************************************************
*
* Objective: Calculate minimum value of a point cloud
* Issues:
*
*******************************************************************************/
void calc_min(struct PointCloud *temp) {

  temp->min[0] = temp->x[0];
  temp->min[1] = temp->y[0];
  temp->min[2] = temp->z[0];

  for(int i = 0; i < temp->count_points; i++){

    if(temp->x[i] < temp->min[0])
      temp->min[0] = temp->x[i];

    if(temp->y[i] < temp->min[1])
      temp->min[1] = temp->y[i];

    if(temp->z[i] < temp->min[2])
      temp->min[2] = temp->z[i];
  }
}
/*******************************************************************************
*
* Objective: Calculate maximum value of a point cloud
* Issues:
*
*******************************************************************************/
void calc_max(struct PointCloud *temp) {

  temp->max[0] = temp->x[0];
  temp->max[1] = temp->y[0];
  temp->max[2] = temp->z[0];

  for(int i = 0; i < temp->count_points; i++){

    if(temp->x[i] > temp->max[0])
      temp->max[0] = temp->x[i];

    if(temp->y[i] > temp->max[1])
      temp->max[1] = temp->y[i];

    if(temp->z[i] > temp->max[2])
      temp->max[2] = temp->z[i];
  }
}
/*******************************************************************************
*
* Objective: Calculate average points of a PointCloud file
* Issues:
*
*******************************************************************************/
void calc_average(struct PointCloud *temp) {

  for(int i = 0; i < temp->count_points; i++){

    temp->average[0] += temp->x[i];
    temp->average[1] += temp->y[i];
    temp->average[2] += temp->z[i];

  }

  temp->average[0] /= temp->count_points;
  temp->average[1] /= temp->count_points;
  temp->average[2] /= temp->count_points;
}
/*******************************************************************************
*
* Objective: Calculate Standard Deviation
* Issues:
*
*******************************************************************************/
void calc_std(struct PointCloud *temp) {

  for(int i = 0; i < temp->count_points; i++){

    temp->std[0] += pow(temp->x[i] - temp->average[0], 2);
    temp->std[1] += pow(temp->y[i] - temp->average[1], 2);
    temp->std[2] += pow(temp->z[i] - temp->average[2], 2);

  }

  temp->std[0] = sqrt(temp->std[0]/temp->count_points);
  temp->std[1] = sqrt(temp->std[1]/temp->count_points);
  temp->std[2] = sqrt(temp->std[2]/temp->count_points);
}
/*******************************************************************************
*
* Objective: Filter negative values of x
* Issues:
*
*******************************************************************************/
void x_negative_filter(struct PointCloud *temp){

  int count = 0;

  for(int i = 0; i < temp->count_points; i++){

    if(temp->x[i] > 0){

      temp->x[count] = temp->x[i];
      temp->y[count] = temp->y[i];
      temp->z[count] = temp->z[i];

      count++;
    }
  }
  temp->count_filtered_points = count;
}
/*******************************************************************************
*
* Objective: Denoise point cloud
* Issues:
*
*******************************************************************************/
void denoise(struct PointCloud *temp){

  int count = 0;

  for(int i = 0; i < temp->count_filtered_points; i++){

    if( temp->x[i] < temp->std[0] && abs(temp->y[i]) < temp->std[1] && abs(temp->z[i]) < temp->std[2] ){
      temp->x[count] = temp->x[i];
      temp->y[count] = temp->y[i];
      temp->z[count] = temp->z[i];
      count++;
    }

  }
  temp->count_filtered_points = count;
}
/*******************************************************************************
*
* Objective: Detect ramps
* Issues: Not implemented. Claculate derivative of Z
*
*******************************************************************************/
void ramps(struct PointCloud *temp){



}
/*******************************************************************************
*
* Objective: Detect ramps
* Issues: Not implemented. Claculate derivative of Z
*
*******************************************************************************/
void process() {

  printf("\n\n------------ Part I - Threads & Semaphores ---------------\n\n");

  struct PointCloud pointCloud; // original file

  /* Reset counter to number of points readed in original files */
  pointCloud.count_points = 0;
  /* Reset counter to number of points writed in filtered files */
  pointCloud.count_filtered_points = 0;

  /* Lock all of the calling process's virtual address space into RAM  Note: requires "sudo" to lock the memory. */
  if( mlockall(MCL_CURRENT | MCL_FUTURE) == -1 )
  errorExit("main->mlockall");

  cpu_set_t mask;
  pthread_attr_t attr[3];
  struct sched_param priority[3];

  /* Set all threads affinity to CPU0 */
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);


  /* One thread for each task: */
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

    /* Scheduler */
    if( pthread_attr_setschedpolicy(&(attr[i]), POLICY2USE) != 0 )
    errorExit("main->pthread_attr_setschedpolicy");


    /* All threads with the same priority value */
    memset(&(priority[i]), 0, sizeof(struct sched_param));
    if( (priority[i].sched_priority = sched_get_priority_max(POLICY2USE)) == -1)
    errorExit("main->sched_get_priority_max");

    printf("Priority of Task %d: %d\n", i+1, priority[i].sched_priority);

    if( pthread_attr_setschedparam(&(attr[i]), &(priority[i])) != 0 )
    errorExit("main->pthread_attr_setschedparam");

  }

  /* Initialize Semaphore */
  sem_init(&mutex, 0, 1);

  /* Create threads for all tasks - Filters */
  // if(pthread_create(&(thread[0]), &(attr[0]), task1, NULL) != 0) // (void *) task3 (void *) &pointCloud void* (*)(task1)
  //   errorExit("main->pthread_create");
  // printf("\nThread 1 was created!\n");
  //
  // if(pthread_create(&(thread[1]), &(attr[1]), task2, NULL) != 0)
  //   errorExit("main->pthread_create");
  // printf("\nThread 2 was created!\n");
  //
  // if(pthread_create(&(thread[2]), &(attr[2]), task3, NULL) != 0)
  //   errorExit("main->pthread_create");
  // printf("Thread 3 was created!\n");
  //
  //
  /* Waits for the ending of each thread tasks - Filters */
  if(pthread_join(thread[0], NULL) != 0)
    errorExit("main->pthread_join");
  //
  // if(pthread_join(thread[1],NULL) != 0)
  //   errorExit("main->pthread_join");
  //
  // if(pthread_join(thread[2],NULL) != 0)
  //   errorExit("main->pthread_join");

  printf("\nWaiting...\n\n");

  /* Final results... for each task */
  printf("Response times:\n");
  //response_times();

  /* Cancel tasks threads */
  for (int i = 0; i < 1; i++) {
    pthread_cancel(thread[i]);
  }
  /* destroy mutex before ending the program */
  sem_destroy(&mutex);
  //exit(EXIT_SUCCESS);
}