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

#define NUM_POINTS 20000
#define NUM_FILES  3

struct Coord{
    float x;
    float y;
    float z;
};

int count_points = 0; //Global variable acessible in all functions
int count_filtered_points = 0;

void read_file(struct Coord *coord, FILE *fptr, int file);
void write_file(struct Coord *coord, FILE *fptr, int file);
void calc_average(struct Coord *coord,float *average, int number_points);
void calc_min(struct Coord *coord, float *min, int number_points);
void calc_max(struct Coord *coord, float *min, int number_points);
void calc_std(struct Coord *coord, float *average, float *std, int number_points);
void x_negative_filter(struct Coord *coord, struct Coord *temp);
void denoise(struct Coord *coord, float *std);


int main(){

    struct Coord coord[NUM_POINTS]; // original file
    struct Coord filter[NUM_POINTS]; // filtered file

    FILE *readfile;
    FILE *writefile;
    float average[3];
    float min[3];
    float max[3];
    float std[3]; //standard deviation

    // Read, Calculate and Write for each File:
    for (int NUMBER_FILE = 1; NUMBER_FILE <= NUM_FILES; NUMBER_FILE++) {

      readfile = (FILE *)malloc(sizeof(FILE));
      read_file(coord, readfile, NUMBER_FILE);
      free(readfile);

      /* --------------------------------------------------------------------------------------------- */
      printf("========================================================\n");

      printf("Number of points: %i\n", count_points);

      calc_average(coord, average, count_points);
      printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

      calc_min(coord, min, count_points);
      printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

      calc_max(coord, max, count_points);
      printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

      calc_std(coord, average, std, count_points);
      printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", std[0], std[1], std[2]);

      printf("========================================================\n");
      /* --------------------------------------------------------------------------------------------- */

      x_negative_filter(coord, filter);

      printf("After filtering: number of points = %i\n", count_filtered_points);

      calc_average(filter, average, count_filtered_points);
      printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

      calc_min(filter, min, count_filtered_points);
      printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

      calc_max(filter, max, count_filtered_points);
      printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

      calc_std(filter, average, std, count_filtered_points);
      printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", std[0], std[1], std[2]);


      printf("========================================================\n");
      /* --------------------------------------------------------------------------------------------- */

      denoise(filter, std);

      printf("STD filtering: number of points = %i\n", count_filtered_points);
      calc_average(filter, average, count_filtered_points);
      printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

      calc_min(filter, min, count_filtered_points);
      printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

      calc_max(filter, max, count_filtered_points);
      printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

      calc_std(filter, average, std, count_filtered_points);
      printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", std[0], std[1], std[2]);

      printf("========================================================\n");
      /* --------------------------------------------------------------------------------------------- */

      writefile = (FILE *)malloc(sizeof(FILE));
      write_file(filter, writefile, NUMBER_FILE);
      free(writefile);

      // Counters points Reset
      count_points = 0;
      count_filtered_points = 0;
    }
    return 0;
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
