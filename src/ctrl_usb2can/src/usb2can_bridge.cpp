#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/wheel_states.hpp"
#include "lib_operator.h"

class USB2CANBridge : public rclcpp::Node {
public:
  USB2CANBridge() : Node("usb2can_bridge") {
    // Parameter for device path
    this->declare_parameter("device_path", "/dev/usb2can_a");
    std::string dev_path = this->get_parameter("device_path").as_string();

    // Open managed connection
    handle_ = op_open(dev_path.c_str());

    if (!handle_ || handle_->fd < 0) {
        RCLCPP_WARN(this->get_logger(), "Could not initially open %s. Will retry automatically.", dev_path.c_str());
    } else {
        RCLCPP_INFO(this->get_logger(), "Bridge started on device: %s", dev_path.c_str());
    }

    // Subscriber for wheel states
    wheel_sub_ = this->create_subscription<robot_interfaces::msg::WheelStates>(
        "wheels", 10, std::bind(&USB2CANBridge::wheel_callback, this, std::placeholders::_1));
  }

  ~USB2CANBridge() {
    if (handle_) {
      op_close_handle(handle_);
    }
  }

private:
  void wheel_callback(const robot_interfaces::msg::WheelStates::SharedPtr msg) {
    if (!handle_) return;

    for (const auto &wheel : msg->wheels) {
      float duty = static_cast<float>(std::max(-100.0, std::min(100.0, wheel.speed * 100.0)));

      // --- Robot B (Crawler) Mapping ---
      if (wheel.name == "right") {
        op_send_duty_auto(handle_, 0x21, duty);
      } else if (wheel.name == "left") {
        op_send_duty_auto(handle_, 0x22, duty);
      }
      // --- Robot A (Swerve) Mapping ---
      else if (wheel.name == "front_left") {
        op_send_duty_auto(handle_, 0x11, duty);
      } else if (wheel.name == "front_right") {
        op_send_duty_auto(handle_, 0x12, duty);
      } else if (wheel.name == "rear_left") {
        op_send_duty_auto(handle_, 0x13, duty);
      } else if (wheel.name == "rear_right") {
        op_send_duty_auto(handle_, 0x14, duty);
      }
      // --- Robot A (Limit Boards) ---
      else if (wheel.name == "limit_15") {
        op_send_direction_auto(handle_, 0x15, static_cast<int8_t>(wheel.speed));
      } else if (wheel.name == "limit_16") {
        op_send_direction_auto(handle_, 0x16, static_cast<int8_t>(wheel.speed));
      } else if (wheel.name == "limit_17") {
        op_send_direction_auto(handle_, 0x17, static_cast<int8_t>(wheel.speed));
      }
    }
  }

  OpHandle *handle_ = nullptr;
  rclcpp::Subscription<robot_interfaces::msg::WheelStates>::SharedPtr wheel_sub_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<USB2CANBridge>());
  rclcpp::shutdown();
  return 0;
}
