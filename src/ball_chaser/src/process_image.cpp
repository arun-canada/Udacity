#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <vector>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Command robot service driving the robot in the specified direction");

    // Request centered joint angles [1.57, 1.57]
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int ball_position;
    int drive_direction; //1 being left, 2 being forward, 3 being right
    bool white_ball_found = false;

       // Loop every pixel and find the white pixel
      for (int i = 0; i < img.height * img.step; i += 3) 
	{
          // Determine if the white pixel is to the left, right or center of the camera
	  ball_position = i % (img.width * 3) / 3;
	  if (img.data[i] == white_pixel && img.data[i + 1] == white_pixel && img.data[i + 2] == white_pixel)
	   {
	    if (ball_position <= 266)
		{
	          drive_direction = 1;
		  white_ball_found = true;
		}
	    else if (ball_position > 266 && ball_position <= 533)
		{
	          drive_direction = 2;
		  white_ball_found = true;
		}
	    else if (ball_position > 533)
		{
	          drive_direction = 3;
		  white_ball_found = true;
		}
	   }
	}
    // If white pixel is not found do not move the robot
    if (white_ball_found == false)
    {
      drive_direction = 0;
    }
     // Call the drive_bot function based on drive_direction.

    if (drive_direction == 0)
    {
        drive_robot(0.0, 0.0); // Robot not commanded
    }
    else if (drive_direction == 1) 
    {
	drive_robot(0.0, 0.5);  // Robot commanded to left
    }
    else if (drive_direction == 2) 
    {
        drive_robot(0.5, 0.0);  // Robot commanded to forward
    }
    else if (drive_direction == 3) 
    {
        drive_robot(0.0, -0.5); // Robot commanded to right
    }

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
