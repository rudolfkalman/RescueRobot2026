#define _USER_MATH_DEFINES // NOLINT(bugprone-reserved-identifier,readability-identifier-naming)
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

// project-specific messages
#include "robot_interfaces/msg/joy_input.hpp"
#include "robot_interfaces/msg/wheel_state.hpp"
#include "robot_interfaces/msg/wheel_states.hpp"

using namespace std::chrono_literals;

class CrawlerControl : public rclcpp::Node {
public:
  CrawlerControl() : Node("crawler_control") {
    publisher_ = this->create_publisher<robot_interfaces::msg::WheelStates>(
        "wheels", 10);

    this->declare_parameter("tracks.left.x", -0.2);
    this->declare_parameter("tracks.right.x", 0.2);

    left_track_x_ = this->get_parameter("tracks.left.x").as_double();
    right_track_x_ = this->get_parameter("tracks.right.x").as_double();

    auto joy_input_callback =
        [this](robot_interfaces::msg::JoyInput::UniquePtr msg) -> void {
      
      double v = msg->l_stick_x;
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
        "joy_input", 10, joy_input_callback);
  }

private:
  double left_track_x_;
  double right_track_x_;

  rclcpp::Subscription<robot_interfaces::msg::JoyInput>::SharedPtr subscription_;
  rclcpp::Publisher<robot_interfaces::msg::WheelStates>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CrawlerControl>());
  rclcpp::shutdown();
  return 0;
}
