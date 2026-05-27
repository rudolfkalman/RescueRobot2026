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

struct Wheel {
  Eigen::Vector2d r;
};

struct WheelOutput {
  double angle;
  double speed;
};

// Joy Subscriber
class SwerveSteer : public rclcpp::Node {
public:
  Wheel load_wheel(const std::string &name) {
    Wheel w{Eigen::Vector2d::Zero()};
    const auto x_name = "wheels." + name + ".x";
    const auto y_name = "wheels." + name + ".y";

    bool loaded_x = this->get_parameter(x_name, w.r.x());
    bool loaded_y = this->get_parameter(y_name, w.r.y());
    if (!loaded_x || !loaded_y) {
      RCLCPP_WARN(
          this->get_logger(),
          "Failed to load wheel parameter(s) for %s (x:%s y:%s). Using default 0.0.",
          name.c_str(), loaded_x ? "ok" : "missing", loaded_y ? "ok" : "missing");
    }
    return w;
  }

  SwerveSteer() : Node("swerve_steer", 
                       rclcpp::NodeOptions()
                        .allow_undeclared_parameters(true)
                        .automatically_declare_parameters_from_overrides(true)) {
    publisher_ = this->create_publisher<robot_interfaces::msg::WheelStates>(
        "/wheels", 10);

    wheels_["front_left"] = load_wheel("front_left");
    wheels_["front_right"] = load_wheel("front_right");
    wheels_["rear_left"] = load_wheel("rear_left");
    wheels_["rear_right"] = load_wheel("rear_right");

    auto joy_input_callback =
        [this](robot_interfaces::msg::JoyInput::UniquePtr msg) -> void {
      /*std::stringstream ss;
      for (auto a : msg->axes)
        ss << a << " ";
      RCLCPP_INFO(this->get_logger(), "Axes: %s", ss.str().c_str());*/

      // V_i = V + omega * r_si
      Eigen::Vector2d velocity(msg->l_stick_x, msg->l_stick_y);
      omega = 2*(msg->r2_trigger - msg->l2_trigger);
      double abs = velocity.norm();

      std::vector<WheelOutput> outputs(4);
      robot_interfaces::msg::WheelStates wheel_status;

      for (size_t i = 0; i < wheel_order_.size(); i++) {

        const auto &name = wheel_order_[i];
        const auto &wheel = wheels_[name];

        Eigen::Vector2d velocity_i =
            velocity +
            Eigen::Vector2d(-omega * wheel.r.y(), omega * wheel.r.x());

        WheelOutput out;
        double angle = std::atan2(velocity_i.y(), velocity_i.x());
        out.angle = angle;
        double speed = velocity_i.norm();
        out.speed = speed;
        outputs[i] = out;

        wheel_status.wheels.push_back(robot_interfaces::msg::WheelState());
        wheel_status.wheels[i].angle = angle;
        wheel_status.wheels[i].speed = speed;
        wheel_status.wheels[i].name = name;  
      }

      this->publisher_->publish(wheel_status);

    };

    subscription_ = this->create_subscription<robot_interfaces::msg::JoyInput>(
        "/joy_input", 10, joy_input_callback);
  }

private:
  double vx = 0.0;
  double vy = 0.0;
  double omega = 0.0;

  std::map<std::string, Wheel> wheels_;

  std::vector<std::string> wheel_order_ = {"front_left", "front_right",
                                           "rear_left", "rear_right"};

  rclcpp::Subscription<robot_interfaces::msg::JoyInput>::SharedPtr
      subscription_;

  rclcpp::Publisher<robot_interfaces::msg::WheelStates>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SwerveSteer>());
  rclcpp::shutdown();
  return 0;
}
