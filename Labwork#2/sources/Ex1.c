#include <stdlib.h>
#include <stdio.h>

#define NUM_POINTS 20000

struct Coord{
    float x;
    float y;
    float z;
};

int count_points = 0; //Global variable acessible in all functions

void read_file(struct Coord *coord, FILE *fptr);
void calc_average(struct Coord *coord,float *average);


int main(){
    struct Coord coord[NUM_POINTS];   
    FILE *fptr;
    float average[3];

    read_file(coord, fptr);
    /* average = (float *)malloc(sizeof(float)); */
    calc_average(coord, average);
    printf("Number of points: %i\n",count_points);
   
    printf("%.4f %.4f %.4f \n", average[0], average[1], average[2]);

    
    return 0;
}

/*******************************************************************************
*
* Objective: Read file from directory and save data into a structure array
* Issues:
*
*******************************************************************************/
void read_file(struct Coord *coord, FILE *fptr){
    
    printf("Opening the file \n");
    fptr = fopen("../resources/point_cloud1.txt","r");  // Open the file
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
    printf("Closing the file \n");
}

/*******************************************************************************
*
* Objective:
* Issues:
*
*******************************************************************************/
void calc_min() {


}
/*******************************************************************************
*
* Objective:
* Issues:
*
*******************************************************************************/
void calc_max() {


}
/*******************************************************************************
*
* Objective: is to alculate average points Cloud
* Issues:
*
*******************************************************************************/
void calc_average(struct Coord *coord, float *average) {

  for(int i = 0; i < count_points; i++){ 
    average[0] += coord[i].x;
    average[1] += coord[i].y;
    average[2] += coord[i].z;
}
    average[0] /= count_points; 
    average[1] /= count_points;
    average[2] /= count_points;
}
/*******************************************************************************
*
* Objective:
* Issues:
*
*******************************************************************************/
void  calc_std() {



}
