// Dual-arm servo bridge: subscribes ArmStates and forwards joint angles
// to an ESP32 over USB serial using the "S a0..a7" bulk command.
#include <array>
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/arm_states.hpp"

#include "lib_servo.h"

namespace ctrl_dualarm {

namespace {
constexpr int kServoCount = 8;
constexpr double kAngleMinDeg = 0.0;
constexpr double kAngleMaxDeg = 180.0;
constexpr double kNeutralDeg = 90.0;
constexpr double kRadToDeg = 180.0 / M_PI;
constexpr double kSendThresholdDeg = 0.5;
constexpr int kKeepalivePeriodMs = 1000;
constexpr int kReopenPeriodMs = 500;
constexpr int kWarnThrottleMs = 1000;
constexpr int kSubscriptionQueueDepth = 10;
}  // namespace

class DualArmBridge : public rclcpp::Node {
 public:
  DualArmBridge() : Node("dualarm_bridge") {
    std::string device_path =
        this->declare_parameter<std::string>("device_path", "/dev/esp_arm8");

    joint_order_ = this->declare_parameter<std::vector<std::string>>(
        "joint_order",
        {"l_joint1", "l_joint2", "l_joint3", "l_joint4",
         "r_joint1", "r_joint2", "r_joint3", "r_joint4"});
    offset_deg_ = this->declare_parameter<std::vector<double>>(
        "offset_deg", std::vector<double>(kServoCount, kNeutralDeg));
    sign_ = this->declare_parameter<std::vector<double>>(
        "sign", std::vector<double>(kServoCount, 1.0));

    angles_deg_.fill(kNeutralDeg);
    for (int i = 0; i < static_cast<int>(joint_order_.size()); ++i) {
      index_[joint_order_[i]] = i;
    }

    handle_ = servo_open(device_path.c_str());
    if (handle_ == nullptr || handle_->fd < 0) {
      RCLCPP_WARN(this->get_logger(),
                  "Could not open %s yet. Will retry automatically.",
                  device_path.c_str());
    } else {
      RCLCPP_INFO(this->get_logger(), "Dual-arm bridge started on %s",
                  device_path.c_str());
    }

    arm_sub_ = this->create_subscription<robot_interfaces::msg::ArmStates>(
        "arm", kSubscriptionQueueDepth,
        std::bind(&DualArmBridge::OnArmStates, this, std::placeholders::_1));

    reopen_timer_ = this->create_wall_timer(
        std::chrono::milliseconds(kReopenPeriodMs),
        std::bind(&DualArmBridge::TryReopen, this));

    keepalive_timer_ = this->create_wall_timer(
        std::chrono::milliseconds(kKeepalivePeriodMs),
        std::bind(&DualArmBridge::OnKeepalive, this));
  }

  ~DualArmBridge() override {
    if (handle_ != nullptr) {
      servo_close(handle_);
    }
  }

 private:
  static double Clamp(double value, double low, double high) {
    return value < low ? low : (value > high ? high : value);
  }

  void TryReopen() {
    if (handle_ != nullptr && handle_->fd < 0) {
      if (servo_try_reopen(handle_)) {
        RCLCPP_INFO(this->get_logger(), "Reconnected to serial device.");
      }
    }
  }

  void SendAll() {
    if (!servo_send_all(handle_, angles_deg_.data())) {
      RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(),
                           kWarnThrottleMs,
                           "Serial write failed; will retry connection.");
    }
  }

  void OnKeepalive() {
    SendAll();
  }

  void OnArmStates(robot_interfaces::msg::ArmStates::SharedPtr msg) {
    bool changed = false;
    for (const auto& joint : msg->joints) {
      const auto it = index_.find(joint.name);
      if (it == index_.end()) {
        continue;
      }
      const int i = it->second;
      if (i >= static_cast<int>(offset_deg_.size()) ||
          i >= static_cast<int>(sign_.size())) {
        continue;
      }
      const double new_deg =
          Clamp(offset_deg_[i] + sign_[i] * (joint.angle * kRadToDeg),
                kAngleMinDeg, kAngleMaxDeg);
      if (std::abs(new_deg - angles_deg_[i]) < kSendThresholdDeg) {
        continue;
      }
      angles_deg_[i] = new_deg;
      changed = true;
    }
    if (changed) {
      SendAll();
    }
  }

  servo_handle_t* handle_ = nullptr;
  std::array<double, kServoCount> angles_deg_{};
  std::vector<std::string> joint_order_;
  std::vector<double> offset_deg_;
  std::vector<double> sign_;
  std::unordered_map<std::string, int> index_;

  // NOLINTNEXTLINE(readability-identifier-naming): rclcpp library typedef name.
  rclcpp::Subscription<robot_interfaces::msg::ArmStates>::SharedPtr arm_sub_;
  rclcpp::TimerBase::SharedPtr reopen_timer_;
  rclcpp::TimerBase::SharedPtr keepalive_timer_;
};

}  // namespace ctrl_dualarm

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ctrl_dualarm::DualArmBridge>());
  rclcpp::shutdown();
  return 0;
}
