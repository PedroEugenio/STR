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

void calc_min() {



}

void calc_max() {


}

void calc_average() {

  for(int i = 0; i < NUM_POINTS; i++){
    sum += coord[i];
  }


}

void  calc_std() {



}
