#define _USER_MATH_DEFINES
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

// Joy Subscriber
class Joy2Twist : public rclcpp::Node {
public:
  Joy2Twist() : Node("joy2twist") {
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    auto joy_callback = [this](sensor_msgs::msg::Joy::UniquePtr msg) -> void {
      /*std::stringstream ss;
      for (auto a : msg->axes)
        ss << a << " ";
      RCLCPP_INFO(this->get_logger(), "Axes: %s", ss.str().c_str());*/
      
      float Lstick_x = -msg->axes[0];
      float Lstick_y = -msg->axes[1];
      float Rstick_x = -msg->axes[3];
      float Rstick_y = -msg->axes[4];

      float Vx = Lstick_y;
      float Vy = Lstick_x;

      auto message = geometry_msgs::msg::Twist();
      message.linear.x = Vx;
      message.linear.y = Vy;
      message.angular.z = Rstick_x * 10;
      this->publisher_->publish(message);
    };

    subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "/joy", 10, joy_callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Joy2Twist>());
  rclcpp::shutdown();
  return 0;
}
