#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/joy_input.hpp"
#include "robot_interfaces/msg/wheel_state.hpp"
#include "robot_interfaces/msg/wheel_states.hpp"

using namespace std::chrono_literals;

class SquareDiffDrive : public rclcpp::Node {
public:
  SquareDiffDrive() : Node("square_diff_drive") {
    publisher_ = this->create_publisher<robot_interfaces::msg::WheelStates>(
        "wheels", 10);

    this->declare_parameter("wheels.front_left.y", 0.15);
    this->declare_parameter("wheels.front_right.y", -0.15);

    left_y_ = this->get_parameter("wheels.front_left.y").as_double();
    right_y_ = this->get_parameter("wheels.front_right.y").as_double();

    auto joy_input_callback =
        [this](robot_interfaces::msg::JoyInput::UniquePtr msg) -> void {
      
      double v = msg->l_stick_x;
      double omega = 2.0 * (msg->r2_trigger - msg->l2_trigger);

      robot_interfaces::msg::WheelStates wheel_status;

      // v_left = v - omega * y_left
      // v_right = v - omega * y_right
      double left_speed = v - omega * left_y_;
      double right_speed = v - omega * right_y_;

      auto add_wheel_state = [&](const std::string &name, double speed) {
        robot_interfaces::msg::WheelState state;
        state.name = name;
        state.speed = speed;
        state.angle = 0.0;
        wheel_status.wheels.push_back(state);
      };

      add_wheel_state("front_left", left_speed);
      add_wheel_state("rear_left", left_speed);
      add_wheel_state("front_right", right_speed);
      add_wheel_state("rear_right", right_speed);

      // --- Limit Board Controls (Robot A) ---
      auto add_limit_cmd = [&](const std::string &name, bool fwd, bool rev) {
        robot_interfaces::msg::WheelState state;
        state.name = name;
        state.speed = fwd ? 1.0 : (rev ? -1.0 : 0.0);
        state.angle = 0.0;
        wheel_status.wheels.push_back(state);
      };

      // Triangle/Cross -> 0x15
      add_limit_cmd("limit_15", msg->btn_triangle, msg->btn_cross);
      // Circle/Square -> 0x16
      add_limit_cmd("limit_16", msg->btn_circle, msg->btn_square);
      // L1/R1 -> 0x17
      add_limit_cmd("limit_17", msg->btn_l1, msg->btn_r1);

      this->publisher_->publish(wheel_status);
    };

    subscription_ = this->create_subscription<robot_interfaces::msg::JoyInput>(
        "joy_input", 10, joy_input_callback);
  }

private:
  double left_y_;
  double right_y_;

  rclcpp::Subscription<robot_interfaces::msg::JoyInput>::SharedPtr subscription_;
  rclcpp::Publisher<robot_interfaces::msg::WheelStates>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SquareDiffDrive>());
  rclcpp::shutdown();
  return 0;
}
