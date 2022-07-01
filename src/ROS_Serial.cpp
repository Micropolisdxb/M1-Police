#include <ROS_Serial.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>

ros::NodeHandle nh;

String freeModeStr = "freeMode";
String donutModeStr = "donutMode";
String spinModeStr = "spinMode";
String idleStr = "brake";
String G_Mode_Msg = "";
String G_Direction_Msg = "";

int G_Emergency_Brake_Msg = 0; // emergency brake is off by default/
int G_Throttle_Msg = 0;
int G_Steering = 0;
int G_Full_Light = 0;
int G_Head_Light = 0;
int G_Siren1_Msg = 0;
int G_Siren2_Msg = 0;
int G_Siren3_Msg = 0;
int G_Siren_Light_Msg = 0;
int G_Disc_Brake_Msg = 0;
int G_Drone_Launch_Msg = 0;

void Drive_Mode_Cb(const std_msgs::String &Mode_Msg)
{
  nh.loginfo("----------------");

  G_Mode_Msg = String(Mode_Msg.data).trim();

  // Debugging
  nh.loginfo((String("Drive Mode = ") + (G_Mode_Msg.c_str())).c_str());
}

void Drive_Direction_Cb(const std_msgs::String &Direction_Msg)
{
  G_Direction_Msg = String(Direction_Msg.data).trim();

  // Debugging
  nh.loginfo((String("Direction = ") + (G_Direction_Msg.c_str())).c_str());
}

void Emergency_Brake_Cb(const std_msgs::Int16 &Em_Brake_Msg)
{
  G_Emergency_Brake_Msg = int(Em_Brake_Msg.data);

  // Debugging
  nh.loginfo((String("Emergency Brake State = ") + String(G_Emergency_Brake_Msg).c_str()).c_str());
}

void Throttle_Cb(const std_msgs::Int16 &Throttle_Msg)
{
  G_Throttle_Msg = int(Throttle_Msg.data);
  nh.loginfo((String("Throttle = ") + String(Throttle_Msg.data).c_str()).c_str());
}

void Steering_Cb(const std_msgs::Int16 &Steering_Msg)
{
  G_Steering = int(Steering_Msg.data);

  // Debugging
  nh.loginfo((String("Steering = ") + String(Steering_Msg.data).c_str()).c_str());
}

void Full_Light_Cb(const std_msgs::Int16 &Light_Full_Msg)
{
  G_Full_Light = Light_Full_Msg.data;
  // Debugging
  nh.loginfo((String("Full Lights = ") + String(Light_Full_Msg.data).c_str()).c_str());
}

void Head_Light_Cb(const std_msgs::Int16 &Light_Head_Msg)
{
  G_Head_Light = Light_Head_Msg.data;
  // Debugging
  nh.loginfo((String("Head Lights = ") + String(Light_Head_Msg.data).c_str()).c_str());
}

void Siren1_Cb(const std_msgs::Int16 &Siren1_Msg)
{
  G_Siren1_Msg = Siren1_Msg.data;
  // Debugging
  nh.loginfo((String("Siren 1 = ") + String(Siren1_Msg.data).c_str()).c_str());
}

void Siren2_Cb(const std_msgs::Int16 &Siren2_Msg)
{
  G_Siren2_Msg = Siren2_Msg.data;
  // Debugging
  nh.loginfo((String("Siren 2 = ") + String(Siren2_Msg.data).c_str()).c_str());
}

void Siren3_Cb(const std_msgs::Int16 &Siren3_Msg)
{
  G_Siren3_Msg = Siren3_Msg.data;
  // Debugging
  nh.loginfo((String("Siren 3 = ") + String(Siren3_Msg.data).c_str()).c_str());
}

void Siren_Light_Cb(const std_msgs::Int16 &Siren_Light_Msg)
{
  G_Siren_Light_Msg = Siren_Light_Msg.data;

  // Debugging
  nh.loginfo((String("Siren Light = ") + String(Siren_Light_Msg.data).c_str()).c_str());
}

void Disc_Brake_Cb(const std_msgs::Int16 &Disc_Brake_Msg)
{
  G_Disc_Brake_Msg = Disc_Brake_Msg.data;

  // Debugging
  nh.loginfo((String("Disc Brake = ") + String(Disc_Brake_Msg.data).c_str()).c_str());
}

void Drone_Launch_Cb(const std_msgs::Int16 &Drone_Launch_Msg)
{
  G_Drone_Launch_Msg = Drone_Launch_Msg.data;

  // Debugging
  nh.loginfo((String("Drone_Launch = ") + String(Drone_Launch_Msg.data).c_str()).c_str());
}

ros::Subscriber<std_msgs::String> Drive_Mode_Subscriber("ros_drive_mode", &Drive_Mode_Cb);
ros::Subscriber<std_msgs::String> Drive_Direction_Subscriber("ros_direction_mode", &Drive_Direction_Cb);

ros::Subscriber<std_msgs::Int16> Throttle_Subscriber("ros_throttle", &Throttle_Cb);
ros::Subscriber<std_msgs::Int16> Steering_Subscriber("ros_steering", &Steering_Cb);
ros::Subscriber<std_msgs::Int16> Full_Light_Subscriber("ros_lights_full", &Full_Light_Cb);
ros::Subscriber<std_msgs::Int16> Head_Light_Subscriber("ros_lights_head", &Head_Light_Cb);
ros::Subscriber<std_msgs::Int16> Siren1_Subscriber("ros_siren1", &Siren1_Cb);
ros::Subscriber<std_msgs::Int16> Siren2_Subscriber("ros_siren2", &Siren2_Cb);
ros::Subscriber<std_msgs::Int16> Siren3_Subscriber("ros_siren3", &Siren3_Cb);
ros::Subscriber<std_msgs::Int16> Siren_Light_Subscriber("ros_siren_light", &Siren_Light_Cb);
ros::Subscriber<std_msgs::Int16> Drone_Launch_Subscriber("ros_drone_launch", &Drone_Launch_Cb);
ros::Subscriber<std_msgs::Int16> Disc_Brake_Subscriber("ros_brake", &Disc_Brake_Cb);
ros::Subscriber<std_msgs::Int16> Emergency_Brake_Subscriber("ros_emergency_brake", &Emergency_Brake_Cb);

void ROS_Init()
{
  nh.initNode();

  nh.subscribe(Drive_Mode_Subscriber);
  nh.subscribe(Drive_Direction_Subscriber);
  nh.subscribe(Steering_Subscriber);
  nh.subscribe(Throttle_Subscriber);
  nh.subscribe(Emergency_Brake_Subscriber);
  nh.subscribe(Full_Light_Subscriber);
  nh.subscribe(Head_Light_Subscriber);
  nh.subscribe(Siren1_Subscriber);
  nh.subscribe(Siren2_Subscriber);
  nh.subscribe(Siren3_Subscriber);
  nh.subscribe(Siren_Light_Subscriber);

  nh.subscribe(Disc_Brake_Subscriber);
  nh.subscribe(Drone_Launch_Subscriber);
}