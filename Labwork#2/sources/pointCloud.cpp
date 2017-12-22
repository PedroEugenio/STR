/************************************************************************
 *
 *  Labwork #2 - Real Time Systems
 *
 *  Group: Frederico Vaz & Pedro Eugénio
 *
 *  Part II
 *
 ************************************************************************/
#include"ros/ros.h"
#include"std_msgs/String.h" // To make cout or ROS_INFO
#include<sensor_msgs/PointCloud.h>
#include<sensor_msgs/PointCloud2.h>
#include<sensor_msgs/point_cloud_conversion.h>


/* Create a ROS Publisher to public a output point cloud message */
ros::Publisher newPointCloud;

/* Create a global message type PointCloud2 */
sensor_msgs::PointCloud2::ConstPtr pointcloud;

/* To run our threads  */
bool runflag = false;


/*******************************************************************************
*
* Objective:
*
* Issues:
*
*******************************************************************************/
void pointcloudCallback(sensor_msgs::PointCloud2::ConstPtr scan_out) {

  //ROS_INFO("Points: ");

  pointcloud = scan_out;

  sensor_msgs::PointCloud output;

  sensor_msgs::convertPointCloud2ToPointCloud(*pointcloud, output);

  //std::cout << "Points: " << scan_out->height*scan_out->width << std::endl;

  std::cout << "Points: " << output.points.size() << std::endl;

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
void task1(sensor_msgs::PointCloud2::ConstPtr pc){

  //Part I 1)

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
void task2(){

  //Part I 2)

}
/*******************************************************************************
*
* Objective: Detect the points that belong to the drivable area with respect to
*            the car i.e LIDAR points that belong to the ground/road
*
* Issues: Not implemented. I put here 2.3) discard the Outliers
*
*******************************************************************************/
void task3(){

  sensor_msgs::PointCloud output;

  output.header = pointcloud->header; //set message header (using the same from /velodyne_points)

  //Part I 3)


  /* Publish/broadcast the message to anyone who is connected */
  newPointCloud.publish(output);
}
/*******************************************************************************
*
* Objective:
*
* Issues:
*
*******************************************************************************/
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



  while(nh.ok()) {
    /*
    ros::ok() will return false if:
    - a SIGINT is received (Ctrl-C)
    - we have been kicked off the network by another node with the same name
    - ros::shutdown() has been called by another part of the application.
    - all ros::NodeHandles have been destroyed
    */

    if(runflag) {
      // task1(pointcloud);
      // task2();
      // task3();
      // runflag = false;
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
