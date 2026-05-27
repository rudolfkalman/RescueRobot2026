#define _USER_MATH_DEFINES // NOLINT(bugprone-reserved-identifier,readability-identifier-naming)
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

// project-specific messages
#include "robot_interfaces/msg/joy_input.hpp"
#include "robot_interfaces/msg/wheel_state.hpp"
#include "robot_interfaces/msg/wheel_states.hpp"

using namespace std::chrono_literals;

// Joy Subscriber
class CrawlerControl : public rclcpp::Node {
public:
  CrawlerControl() : Node("crawler_control", 
                       rclcpp::NodeOptions()
                        .allow_undeclared_parameters(true)
                        .automatically_declare_parameters_from_overrides(true)) {
    publisher_ = this->create_publisher<robot_interfaces::msg::WheelStates>(
        "/wheels", 10);

    // tracks.left.x and tracks.right.x are used to determine the track distance from center
    this->get_parameter_or("tracks.left.x", left_track_x_, -0.2);
    this->get_parameter_or("tracks.right.x", right_track_x_, 0.2);

    auto joy_input_callback =
        [this](robot_interfaces::msg::JoyInput::UniquePtr msg) -> void {
      
      double v = msg->l_stick_x;
      // L2 is msg->l2_trigger, R2 is msg->r2_trigger
      double omega = 2.0 * (msg->r2_trigger - msg->l2_trigger);

      robot_interfaces::msg::WheelStates wheel_status;

      // Left track
      {
        robot_interfaces::msg::WheelState state;
        state.name = "left";
        state.speed = v + omega * left_track_x_;
        state.angle = 0.0;
        wheel_status.wheels.push_back(state);
      }

      // Right track
      {
        robot_interfaces::msg::WheelState state;
        state.name = "right";
        state.speed = v + omega * right_track_x_;
        state.angle = 0.0;
        wheel_status.wheels.push_back(state);
      }

      this->publisher_->publish(wheel_status);

    };

    subscription_ = this->create_subscription<robot_interfaces::msg::JoyInput>(
        "/joy_input", 10, joy_input_callback);
  }

private:
  double left_track_x_;
  double right_track_x_;

  rclcpp::Subscription<robot_interfaces::msg::JoyInput>::SharedPtr
      subscription_;

  rclcpp::Publisher<robot_interfaces::msg::WheelStates>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CrawlerControl>());
  rclcpp::shutdown();
  return 0;
}
