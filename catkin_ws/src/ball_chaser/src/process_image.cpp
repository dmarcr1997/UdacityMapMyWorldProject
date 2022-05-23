#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
  if(lin_x > 0 || ang_z > 0){
  	ROS_INFO_STREAM("Drive Robot Service");
  }
  ball_chaser::DriveToTarget srv;

  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  if(!client.call(srv))
	ROS_ERROR("Failed to call service drive to target");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
	int position;
  	bool found_ball = false;
  	int colVal;
  	float lin_x = 0.0;
  	float ang_z = 0.0;
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height * img.step; i+=3) {
          if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel ) {
            position = i;
            found_ball = true;
            break;
          }
    }
  	if(found_ball){
      ROS_INFO("Ball Found!!\n Bot driving to - linx:%1.2f, angz:%1.2f", lin_x, ang_z);
	  colVal = position%img.step;
      if(colVal > 0 && colVal < img.step * 1/3){
        ang_z = 0.1;
        lin_x = 0.0;
      } else if (colVal > img.step * 2/3){
      	ang_z = -0.1;
        lin_x = 0.0;
      } else if (colVal > img.step *1/3 && colVal < img.step * 2/3) {
      	ang_z = 0.0;
        lin_x = 0.2;
      } else {
      	ang_z = 0.0;
        lin_x = 0.0;
      }
    } else {
    	ang_z = 0.0;
        lin_x = 0.0;
    }
  	
  	drive_robot(lin_x, ang_z);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}