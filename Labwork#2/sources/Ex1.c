/************************************************************************
 *
 *  Labwork #2 - Real Time Systems
 *
 *  Group: Frederico Vaz & Pedro Eugénio
 *
 *  Part I
 *
 ************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

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
struct Coord{
    float x;
    float y;
    float z;
};

/* Counter to number of points readed in original files */
int count_points = 0;
/* Counter to number of points writed in filtered files */
int count_filtered_points = 0;

/* One threads for each file */
pthread_t thread[NUM_FILES];

/* Activation time for each thread */
struct timespec response_time[NUM_FILES] = {{0,0}, {0,0}, {0,0}};

/* Error Message */
#ifndef errorExit
#define errorExit(msg) do{ perror(msg); exit(EXIT_FAILURE); }while(0)
#endif

/* Global declaration */
void* pointCloud1(void *arg);
void* pointCloud2(void *arg);
void* pointCloud3(void *arg);
void execute_math(int number_points);
void response_times();
void read_file(struct Coord *coord, FILE *fptr, int file);
void write_file(struct Coord *coord, FILE *fptr, int file);
void calc_average(struct Coord *coord,float *average, int number_points);
void calc_min(struct Coord *coord, float *min, int number_points);
void calc_max(struct Coord *coord, float *min, int number_points);
void calc_std(struct Coord *coord, float *average, float *std, int number_points);
void x_negative_filter(struct Coord *coord, struct Coord *temp);
void denoise(struct Coord *coord, float *std);


int main(){
    printf("\n\nPart I - Threads & Semaphores\n\n");

    struct Coord coord[NUM_POINTS]; // original file
    struct Coord filter[NUM_POINTS]; // filtered file

    struct timespec current_time;

    /* Lock all of the calling process's virtual address space into RAM  Note: requires "sudo" to lock the memory. */
    if( mlockall(MCL_CURRENT | MCL_FUTURE) == -1 )
      errorExit("main->mloackall");

    cpu_set_t mask;
    pthread_attr_t attr[NUM_FILES];
    struct sched_param priority[NUM_FILES];

    /* Set all threads affinity to CPU0 */
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);


    /* One thread for each file: */
    for (int NUMBER_FILE = 1; NUMBER_FILE <= NUM_FILES; NUMBER_FILE++) {

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

    /* Create threads for all Point Cloud - Executation of the Tasks */
    if(pthread_create(&(thread[0]), &(attr[0]), pointCloud1, NULL) < 0)
      errorExit("main->pthread_create");

    if(pthread_create(&(thread[1]), &(attr[1]), pointCloud2, NULL) < 0)
      errorExit("main->pthread_create");

    if(pthread_create(&(thread[2]), &(attr[2]), pointCloud3, NULL) < 0)
      errorExit("main->pthread_create");


    /* Final results... for each Point Cloud */
    do {
      printf("\n\nWaiting...\n\n");
      /* Main thread go to sleep when wake up it will cancel tasks threads */
      sleep(FINISH);

      for (int i = 0; i < NUM_FILES; i++) {
        pthread_cancel(thread[i]);
      }
      printf("Inverse RMPO response times:\n");
      response_times();

      exit(EXIT_SUCCESS);
    } while(1);

    /* For each file to read, calculate and write: */
    for (int NUMBER_FILE = 1; NUMBER_FILE <= NUM_FILES; NUMBER_FILE++) {

    }
}
void* pointCloud1(void *arg) {

  FILE *readfile;
  FILE *writefile;

  /* Read orginal file */
  readfile = (FILE *)malloc(sizeof(FILE));
  read_file(coord, readfile, POINTCLOUD1);
  free(readfile);

  execute_math(count_points);

  /* Removing axis X negavtive Points */
  x_negative_filter(coord, filter);
  printf("After filtering...\n\n");
  execute_math(count_filtered_points);

  /* STD filtering - Removing Noising Points */
  denoise(filter, std);
  printf("After filtering...\n\n");
  execute_math(count_filtered_points);

  /* Store in other file */
  writefile = (FILE *)malloc(sizeof(FILE));
  write_file(filter, writefile, NUMBER_FILE);
  free(writefile);

  /* Counters points Reset */
  count_points = 0;
  count_filtered_points = 0;
}
void* pointCloud2(void *arg) {

}
void* pointCloud3(void *arg) {

}
/*******************************************************************************
*
* Objective: Math tasks to do in each Point Cloud
* Issues:
*
*******************************************************************************/
void execute_math(int number_points) {

  float average[3];
  float min[3];
  float max[3];
  float std[3]; //standard deviation

  printf("========================================================\n");

  printf("Number of points: %i\n", number_points);

  calc_average(coord, average, number_points);
  printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

  calc_min(coord, min, number_points);
  printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

  calc_max(coord, max, number_points);
  printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

  calc_std(coord, average, std, number_points);
  printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", std[0], std[1], std[2]);

}
/*******************************************************************************
*
* Objective: Computational time for each Task
* Issues:
*
*******************************************************************************/
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
/*******************************************************************************
*
* Objective: Read file from directory and save data into a structure array
* Notes:
* int file - number of the file required
*******************************************************************************/
void read_file(struct Coord *coord, FILE *fptr, int file){
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
        fscanf(fptr, "%f %f %f", &coord[count_points].x, &coord[count_points].y, &coord[count_points].z);
        //printf("%.4f %.4f %.4f \n", coord[count_points].x, coord[count_points].y, coord[count_points].z);
        count_points++;  // last line of coord is all 0
    }
    count_points-=1;

    fclose(fptr);
}

/*******************************************************************************
*
* Objective: Save filtered data into a new file
* Notes:
* int file - number of the file
*******************************************************************************/
void write_file(struct Coord *coord, FILE *fptr, int file){
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
    for(int i=0; i<count_filtered_points; i++){
        // Saves the values from .txt file to the variables
        fprintf(fptr, "%f %f %f\n", coord[i].x, coord[i].y, coord[i].z);

    }

    fclose(fptr);
}
/*******************************************************************************
*
* Objective: Calculate minimum value of a point cloud
* Issues:
*
*******************************************************************************/
void calc_min(struct Coord *coord, float *min, int number_points) {
    min[0] = coord[0].x;
    min[1] = coord[0].y;
    min[2] = coord[0].z;
    for(int i = 0; i < number_points; i++){
        if(coord[i].x<min[0])
            min[0] = coord[i].x;
        if(coord[i].y<min[1])
            min[1] = coord[i].y;
        if(coord[i].z<min[2])
            min[2] = coord[i].z;
    }
}
/*******************************************************************************
*
* Objective: Calculate maximum value of a point cloud
* Issues:
*
*******************************************************************************/
void calc_max(struct Coord *coord, float *max, int number_points) {
    max[0] = coord[0].x;
    max[1] = coord[0].y;
    max[2] = coord[0].z;
    for(int i = 0; i < number_points; i++){
        if(coord[i].x>max[0])
            max[0] = coord[i].x;
        if(coord[i].y>max[1])
            max[1] = coord[i].y;
        if(coord[i].z>max[2])
            max[2] = coord[i].z;
    }

}
/*******************************************************************************
*
* Objective: Calculate average points of a PointCloud file
* Issues:
*
*******************************************************************************/
void calc_average(struct Coord *coord, float *average, int number_points) {

    for(int i = 0; i < number_points; i++){
        average[0] += coord[i].x;
        average[1] += coord[i].y;
        average[2] += coord[i].z;
    }
        average[0] /= number_points;
        average[1] /= number_points;
        average[2] /= number_points;
    }
/*******************************************************************************
*
* Objective: Calculate Standard Deviation
* Issues:
*
*******************************************************************************/
void calc_std(struct Coord *coord, float *average, float *std,  int number_points) {

    for(int i = 0; i < number_points; i++){
        std[0] += pow(coord[i].x - average[0],2);
        std[1] += pow(coord[i].y - average[1],2);
        std[2] += pow(coord[i].z - average[2],2);
    }

    std[0] = sqrt(std[0]/number_points);
    std[1] = sqrt(std[1]/number_points);
    std[2] = sqrt(std[2]/number_points);
}
/*******************************************************************************
*
* Objective: Filter negative values of x
* Issues:
*
*******************************************************************************/
void x_negative_filter(struct Coord *coord, struct Coord *temp){

    for(int i = 0; i < count_points; i++){
        if(coord[i].x>0){
            temp[count_filtered_points].x = coord[i].x;
            temp[count_filtered_points].y = coord[i].y;
            temp[count_filtered_points].z = coord[i].z;
            count_filtered_points++;
        }
    }

}
/*******************************************************************************
*
* Objective: Denoise point cloud
* Issues:
*
*******************************************************************************/
void denoise(struct Coord *coord, float *std){
   int temp=0;
   for(int i = 0; i < count_filtered_points; i++){
        if( coord[i].x<std[0] && abs(coord[i].y)<std[1] && abs(coord[i].z<std[2]) ){
            coord[temp].x = coord[i].x; // Devemos fazer isto para a filter certo? Vai sempre substituíndo na struct, e a filter deixa de existir
            coord[temp].y = coord[i].y;
            coord[temp].z = coord[i].z;
            temp++;
        }
    }
    count_filtered_points=temp;
}
/*******************************************************************************
*
* Objective: Detect ramps
* Issues: Not implemented. Claculate derivative of Z
*
*******************************************************************************/
void ramps(struct Coord *coord){

}
