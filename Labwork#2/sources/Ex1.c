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
void calc_min(struct Coord *coord, float *min);
void calc_max(struct Coord *coord, float *min);


int main(){
    struct Coord coord[NUM_POINTS];   
    FILE *fptr;
    float average[3];
    float min[3];
    float max[3];

    fptr = (FILE *)malloc(sizeof(FILE));
    read_file(coord, fptr);
    free(fptr);
    calc_average(coord, average);
    printf("Number of points: %i\n",count_points);
    printf("Average Value - x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

    calc_min(coord, min);
    printf("Minimum Value - x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

    calc_max(coord, max);
    printf("Maximum Value - x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

    
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
* Objective: Calculate minimum value of a point cloud
* Issues:
*
*******************************************************************************/
void calc_min(struct Coord *coord, float *min) {
    min[0] = coord[0].x;
    min[1] = coord[0].y;
    min[2] = coord[0].z;
    for(int i = 1; i < count_points; i++){ 
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
void calc_max(struct Coord *coord, float *max) {
    max[0] = coord[0].x;
    max[1] = coord[0].y;
    max[2] = coord[0].z;
    for(int i = 1; i < count_points; i++){ 
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
