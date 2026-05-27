#define _USER_MATH_DEFINES // NOLINT(bugprone-reserved-identifier,readability-identifier-naming)
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/string.hpp"

// project-specific messages
#include "robot_interfaces/msg/joy_input.hpp"

using namespace std::chrono_literals;

// Joy Subscriber
class PS5JoyParser : public rclcpp::Node {
public:
  PS5JoyParser() : Node("ps5_joy_parser") {
    publisher_ = this->create_publisher<robot_interfaces::msg::JoyInput>(
        "/joy_input", 10);
    auto joy_callback = [this](sensor_msgs::msg::Joy::UniquePtr msg) -> void {
      /*std::stringstream ss;
      for (auto a : msg->axes)
        ss << a << " ";
      RCLCPP_INFO(this->get_logger(), "Axes: %s", ss.str().c_str());*/

      // --- アナログ入力 (Axes) の取得 ---
      // 極性：左/上が 正(+)、右/下が 負(-)
      double l_stick_y = msg->axes[0];  // 左スティック左右
      double l_stick_x = msg->axes[1];  // 左スティック上下
      double l2_trigger = msg->axes[2]; // L2トリガー (1.0 〜 -1.0)
      double r_stick_y = msg->axes[3];  // 右スティック左右
      double r_stick_x = msg->axes[4];  // 右スティック上下
      double r2_trigger = msg->axes[5]; // R2トリガー (1.0 〜 -1.0)
      double dpad_y = msg->axes[6];     // 十字キー左右
      double dpad_x = msg->axes[7];     // 十字キー上下

      // --- ボタン入力 (Buttons) の取得 ---
      // 0: 離している / 1: 押している
      bool btn_cross = msg->buttons[0];    // ×
      bool btn_circle = msg->buttons[1];   // ○
      bool btn_triangle = msg->buttons[2]; // △
      bool btn_square = msg->buttons[3];   // □
      bool btn_l1 = msg->buttons[4];       // L1
      bool btn_r1 = msg->buttons[5];       // R1
      bool btn_create = msg->buttons[8];   // クリエイト (左)
      bool btn_options = msg->buttons[9];  // オプション (右)
      bool btn_ps = msg->buttons[10];      // PSボタン
      bool btn_l3 = msg->buttons[11];      // 左スティック押し込み
      bool btn_r3 = msg->buttons[12];      // 右スティック押し込み

      auto message = robot_interfaces::msg::JoyInput();

      message.l_stick_x = l_stick_x;
      message.l_stick_y = l_stick_y;

      message.r_stick_x = r_stick_x;
      message.r_stick_y = r_stick_y;

      message.l2_trigger = l2_trigger;
      message.r2_trigger = r2_trigger;

      message.dpad_x = dpad_x;
      message.dpad_y = dpad_y;

      message.btn_cross = btn_cross;
      message.btn_circle = btn_circle;
      message.btn_triangle = btn_triangle;
      message.btn_square = btn_square;

      message.btn_l1 = btn_l1;
      message.btn_r1 = btn_r1;

      message.btn_create = btn_create;
      message.btn_options = btn_options;

      message.btn_ps = btn_ps;

      message.btn_l3 = btn_l3;
      message.btn_r3 = btn_r3;
      this->publisher_->publish(message);
    };

    subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "/joy", 10, joy_callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
  rclcpp::Publisher<robot_interfaces::msg::JoyInput>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PS5JoyParser>());
  rclcpp::shutdown();
  return 0;
}
