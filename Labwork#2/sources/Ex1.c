#include <stdio.h>

struct Coord{
    float x;
    float y;
    float z;
};

int main(){
    struct Coord coord[];
    printf("Opening the file \n");
    FILE *fptr;
    fptr = fopen("point_cloud.txt","r");  // Open the file
    if(fptr == NULL){
        perror("fopen()");
        exit(1);
    }

    // Verify if the document reached to an end
    while( !feof (fptr) ){
        // Saves the values from .dat file to the variables
        fscanf(fptr, "%f %f %f %f", &coord[i].x, &line_body[i].y, &line_body[i].z, &line_body[i].m);
        //printf("%.1f %.1f %.1f %.1f \n", line_body[i].x, line_body[i].y, line_body[i].z, line_body[i].m);
        i++;
    }

    fclose(fptr);
    printf("Closing the file \n");
    return 0;
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
float * calc_average() {

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
