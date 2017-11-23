#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_POINTS 20000
#define NUMBER_FILE 3

struct Coord{
    float x;
    float y;
    float z;
};

int count_points = 0; //Global variable acessible in all functions
int count_filtered_points = 0;

void read_file(struct Coord *coord, FILE *fptr, int file);
void write_file(struct Coord *coord, FILE *fptr, int file);
void calc_average(struct Coord *coord,float *average);
void calc_min(struct Coord *coord, float *min);
void calc_max(struct Coord *coord, float *min);
void calc_std(struct Coord *coord, float *average, float *s); 
struct Coord x_negative_filter(struct Coord *coord);



int main(){
    struct Coord coord1[NUM_POINTS]; //file 1
    struct Coord coord2[NUM_POINTS]; //file 2
    struct Coord coord3[NUM_POINTS]; //file 3  
    FILE *readfile;
    FILE *writefile;
    float average[3];
    float min[3];
    float max[3];
    float s[3]; //standard deviation

    struct Coord filter;

    readfile = (FILE *)malloc(sizeof(FILE));
    read_file(coord1, readfile, NUMBER_FILE);
    free(readfile);
    printf("Number of points: %i\n",count_points);

    calc_average(coord1, average);
    printf("Average Value :: x:%.4f y:%.4f z:%.4f\n", average[0], average[1], average[2]);

    calc_min(coord1, min);
    printf("Minimum Value :: x:%.4f y:%.4f z:%.4f \n", min[0], min[1], min[2]);

    calc_max(coord1, max);
    printf("Maximum Value :: x:%.4f y:%.4f z:%.4f \n", max[0], max[1], max[2]);

    calc_std(coord1, average, s);
    printf("Standard Deviation Value :: x:%.4f y:%.4f z:%.4f \n", s[0], s[1], s[2]);

    printf("========================================================\n");
    filter = x_negative_filter(coord1);
    printf("After filtering: number of points = %i\n", count_filtered_points);

    writefile = (FILE *)malloc(sizeof(FILE));
    write_file(&filter, writefile, NUMBER_FILE);
    free(writefile);
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
        printf("File point_cloud1 open\n");
    }
    if(file==2){
        fptr = fopen("../resources/point_cloud2.txt","r");  // Open the file 2
        printf("File point_cloud2 open\n");
    }
    if(file==3){
        fptr = fopen("../resources/point_cloud3.txt","r");  // Open the file 3
        printf("File point_cloud3 open\n");
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
        printf("File point_cloud1 open\n");
    }
    if(file==2){
        fptr = fopen("../resources/point_cloud2_filtered.txt","w");  // Open the file 2
        printf("File point_cloud2 open\n");
    }
    if(file==3){
        fptr = fopen("../resources/point_cloud3_filtered.txt","w");  // Open the file 3
        printf("File point_cloud3 open\n");
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
* Objective: Calculate Standard Deviation
* Issues:
*
*******************************************************************************/
void calc_std(struct Coord *coord, float *average, float *s) {

    for(int i = 0; i < count_points; i++){ 
        s[0] += pow(coord[i].x - average[0],2);
        s[1] += pow(coord[i].y - average[1],2);
        s[2] += pow(coord[i].z - average[2],2);
    }

    s[0] = sqrt(s[0]/count_points);
    s[1] = sqrt(s[1]/count_points);
    s[2] = sqrt(s[2]/count_points);
}

/*******************************************************************************
*
* Objective: Filter negative values of x
* Issues:
*
*******************************************************************************/
struct Coord x_negative_filter(struct Coord *coord){
    struct Coord temp[count_points];

    for(int i = 1; i < count_points; i++){ 
        if(coord[i].x>=0){
            temp[i].x = coord[i].x;
            temp[i].y = coord[i].y;
            temp[i].z = coord[i].z;
            count_filtered_points++;
        }
    }

    return *temp;
}
