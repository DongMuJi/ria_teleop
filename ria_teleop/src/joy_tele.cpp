#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <robotnik_msgs/ptz.h>

int statu=0;
 
class TeleopTurtle
{
public:
  TeleopTurtle();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle nh_;

  int linear_, angular_, tilt_, pan_;
  double l_scale_=1; 
  double a_scale_=1;
  ros::Publisher vel_pub_ , ptz_pub_;
  ros::Subscriber joy_sub_;

};


TeleopTurtle::TeleopTurtle():
  linear_(4), angular_(3), tilt_(1), pan_(0)
{

  nh_.param("axis_linear", linear_, linear_);
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("ptz_tilt", tilt_, tilt_);
  nh_.param("ptz_pan", pan_, pan_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);


  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);

  ptz_pub_ = nh_.advertise<robotnik_msgs::ptz>("/axis_camera/ptz_command", 1);

  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopTurtle::joyCallback, this);

}

void TeleopTurtle::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
if (joy->buttons[4])
{
  geometry_msgs::Twist twist;
  twist.angular.z = a_scale_*joy->axes[angular_]*1.5;
  twist.linear.x = l_scale_*joy->axes[linear_]*0.7;
  vel_pub_.publish(twist);
}
else if (joy->axes[2]==-1)
{
  geometry_msgs::Twist twist;
  twist.angular.z = a_scale_*joy->axes[angular_]*3.0;
  twist.linear.x = l_scale_*joy->axes[linear_]*1.5;
  vel_pub_.publish(twist);
}
else if (joy->buttons[5])
{
  robotnik_msgs::ptz ptz_value;
  if(statu==0)
  {
    ptz_value.pan = joy->axes[pan_]*0.02;
    ptz_value.tilt = -joy->axes[tilt_]*0.02;
    ptz_value.zoom = 0.0;
    ptz_value.relative = false;
  }
  if(joy->axes[pan_]!= 0 || joy->axes[tilt_]!= 0 && statu==0 )
  {
    statu=1;
    ptz_value.pan = joy->axes[pan_]*0.05;
    ptz_value.tilt = -joy->axes[tilt_]*0.05;
    ptz_value.zoom = 0.0;
    ptz_value.relative = false;
  }
  if(statu==1)
  {
    ptz_value.pan = joy->axes[pan_]*0.2;
    ptz_value.tilt = -joy->axes[tilt_]*0.2;
    ptz_value.zoom = 0.0;
    ptz_value.relative = true;
  }
    ptz_pub_.publish(ptz_value);
 }
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "teleop_turtle");
  TeleopTurtle teleop_turtle;

  ros::spin();
}
