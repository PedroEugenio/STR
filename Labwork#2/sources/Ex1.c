/************************************************************************
 *
 *  Labwork #2 - Real Time Systems
 *
 *  Group: Frederico Vaz & Pedro Eugénio
 *
 *  Part I
 *
 ************************************************************************/
#define _GNU_SOURCE

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
#include "../resources/timespec.h"

/* MAX number of Points that our program can read */
#define NUM_POINTS   20000
/* MAX number of files that our programa can read */
#define NUM_FILES    3

#define POINTCLOUD1  1
#define POINTCLOUD2  1
#define POINTCLOUD3  3

/* Scheduler RMPO */
#define POLICY2USE SCHED_FIFO //SCHED_RR // SCHED_FIFO

/* Finish Time of threads in s */
#define FINISH       10

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

/* One threads for each file */
pthread_t thread[NUM_FILES];

/* Activation time for each thread */
struct timespec response_time[NUM_FILES] = {{0,0}, {0,0}, {0,0}};

struct timespec computation_time[NUM_FILES];

/* Error Message */
#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Global declaration */
void* task1(struct PointCloud *temp);
void* task2(struct PointCloud *temp);
void* task3(struct PointCloud *temp);

void execute_math(struct PointCloud *temp);
//void response_time();

void read_file(struct PointCloud *temp, FILE *fptr, int file);
void write_file(struct PointCloud *temp, FILE *fptr, int file);

void calc_average(struct PointCloud *temp);
void calc_min(struct PointCloud *temp);
void calc_max(struct PointCloud *temp);
void calc_std(struct PointCloud *temp);

void x_negative_filter(struct PointCloud *temp);
void denoise(struct PointCloud *temp);
void ramps(struct PointCloud *temp);

// TESTING
int NUMBER_FILE = 1;

int main(){
  printf("\n\nPart I - Threads & Semaphores\n\n");

  /* For each file to read, calculate and write */
  // Isto não muito dinâmico, deviamos passar o nome ficheiro com queremos trabalhar
  // até está na alinea 1
  //for (int NUMBER_FILE = 1; NUMBER_FILE <= NUM_FILES; NUMBER_FILE++) {

    struct PointCloud pointCloud; // original file
    //struct Coord filter[NUM_POINTS]; // filtered file

    struct timespec current_time;

    /* Reset counter to number of points readed in original files */
    pointCloud.count_points = 0;
    /* Reset counter to number of points writed in filtered files */
    pointCloud.count_filtered_points = 0;

    /* Lock all of the calling process's virtual address space into RAM  Note: requires "sudo" to lock the memory. */
    if( mlockall(MCL_CURRENT | MCL_FUTURE) == -1 )
    errorExit("main->mloackall");

    cpu_set_t mask;
    pthread_attr_t attr[3];
    struct sched_param priority[3];

    /* Set all threads affinity to CPU0 */
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);


    /* One thread for each task: */
    for (int i = 1; i <= 3; i++) {

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

    /* Create threads for all tasks - Filters */
    // if(pthread_create(&(thread[0]), &(attr[0]), task1(&pointCloud), NULL) < 0)
    //   errorExit("main->pthread_create");
    //
    // if(pthread_create(&(thread[1]), &(attr[1]), task2(&pointCloud), NULL) < 0)
    //   errorExit("main->pthread_create");
    //
    // if(pthread_create(&(thread[2]), &(attr[2]), task3(&pointCloud), NULL) < 0)
    //   errorExit("main->pthread_create");

    // Só para testar: vou chamar aqui as tasks
    task1(&pointCloud);
    task2(&pointCloud);
    task3(&pointCloud);



    /* Final results... for each task */
    do {
      printf("\n\nWaiting...\n\n");
      /* Main thread go to sleep when wake up it will cancel tasks threads */
      //sleep(FINISH);

      // for (int i = 0; i < 3; i++) {
      //   pthread_cancel(thread[i]);
      // }
      //printf("Inverse RMPO response times:\n");
      //response_times();

      exit(EXIT_SUCCESS);
    } while(1);


  //}
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
void* task1(struct PointCloud *temp) {

  FILE *readfile;

  // AQUI DEVE INICIAR O SEMAPHORE

  /* Read orginal file */
  readfile = (FILE *)malloc(sizeof(FILE));
  read_file(temp, readfile, POINTCLOUD1);
  free(readfile);

  printf("Number of points: %i\n", temp->count_points);

  calc_average(temp);
  printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", temp->average[0], temp->average[1],temp->average[2]);

  calc_min(temp);
  printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", temp->min[0], temp->min[1], temp->min[2]);

  calc_max(temp);
  printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", temp->max[0], temp->max[1], temp->max[2]);

  calc_std(temp);
  printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", temp->std[0], temp->std[1], temp->std[2]);

  //execute_math(temp);

  /* Counters points Reset */
  // temp->count_points = 0;
  // temp->count_filtered_points = 0;

  // AQUI DEVE FECHAR O SEMAPHORE
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
void* task2(struct PointCloud *temp) { // struct Coord coord

  /* Removing axis X negative Points */
  // for(int i = 0; i < count_points; i++){
  //     if(coord[i].x>0){
  //         temp[count_filtered_points].x = coord[i].x;
  //         temp[count_filtered_points].y = coord[i].y;
  //         temp[count_filtered_points].z = coord[i].z;
  //         count_filtered_points++;
  //     }
  // } // Podemos passar para aqui a função ? o.O (se não precisamos de calcular a math sempre)

  x_negative_filter(temp);

  /* Clusters were removed above too */

  /* Counters points Reset */
  //temp->count_points = temp->count_filtered_points;

  //printf("After filtering...\n\n");
  //execute_math(count_filtered_points);

}
/*******************************************************************************
*
* Objective: Detect the points that belong to the drivable area with respect to
*            the car i.e LIDAR points that belong to the ground/road
*
* Issues: Not implemented. I put here 2.3) discard the Outliers
*
*******************************************************************************/
void* task3(struct PointCloud *temp) { // struct Coord coord

  FILE *writefile;

  /* STD filtering - Removing Noising Points */
  denoise(temp);

  //printf("After filtering...\n\n");
  //execute_math(count_filtered_points);

  /* Store in other file */
  writefile = (FILE *)malloc(sizeof(FILE));
  write_file(temp, writefile, NUMBER_FILE);
  free(writefile);

}
/*******************************************************************************
*
* Objective: Math tasks to do in each Point Cloud
* Issues:
*
*******************************************************************************/
void execute_math(struct PointCloud *temp) {

  printf("========================================================\n");

  printf("Number of points: %i\n", temp->count_points);

  calc_average(temp);
  printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", temp->average[0], temp->average[1], temp->average[2]);

  calc_min(temp);
  printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", temp->min[0], temp->min[1], temp->min[2]);

  calc_max(temp);
  printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", temp->max[0], temp->max[1], temp->max[2]);

  calc_std(temp);
  printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", temp->std[0], temp->std[1], temp->std[2]);

}
/*******************************************************************************
*
* Objective: Computational time for each Task
* Issues:
*
*******************************************************************************/
// void response_times() {
//   /* Print response_times */
//   printf("TASK 1: %LFs or %LFms\n", time2s(response_time[0]), time2ms(response_time[0]));
//   printf("TASK 2: %LFs or %LFms\n", time2s(response_time[1]), time2ms(response_time[1]));
//   printf("TASK 3: %LFs or %LFms\n", time2s(response_time[2]), time2ms(response_time[2]));
//
//   /* Clear responses times */
//   for (int i = 0; i < TASKS; i++) {
//     response_time[i] = SET(0, 0);
//   }
// }
/*******************************************************************************
*
* Objective: Read file from directory and save data into a structure array
* Notes: int file - number of the file required
*
*******************************************************************************/
void read_file(struct PointCloud *temp, FILE *fptr, int file){ // mudar o int file para uma string
    if(file==1){
        fptr = fopen("../resources/point_cloud1.txt","r");  // Open the file 1
        printf("\nReading point_cloud1.txt\n\n");
    }
    if(file==2){
        fptr = fopen("../resources/point_cloud2.txt","r");  // Open the file 2
        printf("\nReading point_cloud2.txt\n\n");
    }
    if(file==3){
        fptr = fopen("../resources/point_cloud3.txt","r");  // Open the file 3
        printf("\nReading point_cloud3.txt\n\n");
    }
    if(fptr == NULL){
        perror("fopen()");
        exit(1);
    }

    // Verify if the document reached to an end
    while( !feof (fptr) ){
        // Saves the values from .txt file to the variables
        float _x = 0.0, _y = 0.0, _z = 0.0;
        fscanf(fptr, "%f %f %f", &_x, &_y, &_z);

        temp->x[temp->count_points] = _x;
        temp->y[temp->count_points] = _y;
        temp->z[temp->count_points] = _z;

        temp->count_points++;  // last line of coord is all 0
    }
    temp->count_points-=1;

    fclose(fptr);
}

/*******************************************************************************
*
* Objective: Save filtered data into a new file
* Notes:
* int file - number of the file
*******************************************************************************/
void write_file(struct PointCloud *temp, FILE *fptr, int file){

    if(file==1){
        fptr = fopen("../resources/point_cloud1_filtered.txt","w");  // Open the file 1
        printf("\nWriting to point_cloud1_filtered.txt\n\n");
    }
    if(file==2){
        fptr = fopen("../resources/point_cloud2_filtered.txt","w");  // Open the file 2
        printf("\nWriting to point_cloud2_filtered.txt\n\n");
    }
    if(file==3){
        fptr = fopen("../resources/point_cloud3_filtered.txt","w");  // Open the file 3
        printf("\nWriting to point_cloud3_filtered.txt\n\n");
    }
    if(fptr == NULL){
        perror("fopen()");
        exit(1);
    }

    // Verify if the document reached to an end
    for(int i = 0; i < temp->count_filtered_points; i++){
        // Saves the values from .txt file to the variables
        fprintf(fptr, "%f %f %f\n", temp->x[i], temp->y[i], temp->z[i]);

    }

    fclose(fptr);
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
