#include <stdlib.h>
#include <stdio.h>

#define NUM_POINTS 20000

struct Coord{
    float x;
    float y;
    float z;
};


void read_file(struct Coord *coord, FILE *fptr);


int main(){
    struct Coord coord[NUM_POINTS];
    
    FILE *fptr;

    read_file(coord, fptr);
    
    return 0;
}

/*******************************************************************************
*
* Objective: Read file from directory and save data into a structure array
* Issues:
*
*******************************************************************************/
void read_file(struct Coord *coord, FILE *fptr){
    int i=0;
    printf("Opening the file \n");
    fptr = fopen("../resources/point_cloud1.txt","r");  // Open the file
    if(fptr == NULL){
        perror("fopen()");
        exit(1);
    }

    // Verify if the document reached to an end
    while( !feof (fptr) ){ 
        // Saves the values from .txt file to the variables
        fscanf(fptr, "%f %f %f", &coord[i].x, &coord[i].y, &coord[i].z);
        printf("%.4f %.4f %.4f \n", coord[i].x, coord[i].y, coord[i].z);
        i++;
    }
  
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
float * calc_average(struct Coord *coord) {

  float average[3]; //sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;

  for(int i = 0; i < NUM_POINTS; i++){
    average[0] += coord[i].x;
    average[1] += coord[i].y;
    average[2] += coord[i].z;
  }
  return average;
}
/*******************************************************************************
*
* Objective:
* Issues:
*
*******************************************************************************/
void  calc_std() {



}
