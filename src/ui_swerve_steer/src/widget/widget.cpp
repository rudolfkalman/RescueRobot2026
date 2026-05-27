/*
 * Copyright (c) 2024 NITK.K ROS-Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define _USE_MATH_DEFINES
#include "widget/widget.hpp"
#include <QPainter>
#include <cmath>
#include <vector>

namespace ui_swerve_steer
{

  SwerveSteerWidget::SwerveSteerWidget(QWidget * parent)
: rviz_common::Panel(parent)
{
  // UIの初期化
  ui.setupUi(this);
  // ボタンがクリックされたときの処理を設定
  //connect(ui.swerve_steer_push_button, &QPushButton::clicked, this, &SwerveSteerWidget::onPushButtonClicked);
}

void SwerveSteerWidget::onInitialize()
{
  auto node = this->getDisplayContext()->getRosNodeAbstraction().lock()->get_raw_node();

  // デバッグ: 利用可能な全パラメータを表示
  auto param_names = node->list_parameters({}, 0).names;
  RCLCPP_INFO(node->get_logger(), "UI: Available parameters count: %zu", param_names.size());
  for (const auto & name : param_names) {
    RCLCPP_INFO(node->get_logger(), "UI: Found parameter: %s", name.c_str());
  }

  // パブリッシャの初期化
  qt_node_pub_handler_.setRosNodePtr(node);
  qt_node_pub_handler_.initializePublisher("swerve_steer_topic");

  // サブスクライバの初期化
  qt_node_sub_handler_.setRosNodePtr(node);
  qt_node_sub_handler_.initializeSubscription("/wheels");

  // パラメータのロード
  std::vector<std::string> wheel_names = {"front_left", "front_right", "rear_left", "rear_right"};
  for (const auto & name : wheel_names) {
    WheelConfig config;
    config.x = 0.0;
    config.y = 0.0;

    std::string x_param_name = "wheels." + name + ".x";
    std::string y_param_name = "wheels." + name + ".y";

    // パラメータの宣言（オーバーライドがある場合はその値が初期値になる）
    try {
      if (!node->has_parameter(x_param_name)) {
        node->declare_parameter(x_param_name, 0.0);
      }
      if (!node->has_parameter(y_param_name)) {
        node->declare_parameter(y_param_name, 0.0);
      }
    } catch (const rclcpp::exceptions::ParameterAlreadyDeclaredException & e) {
      // 既に宣言されている場合はスキップ
    }

    node->get_parameter(x_param_name, config.x);
    node->get_parameter(y_param_name, config.y);
    
    RCLCPP_INFO(node->get_logger(), "UI: Loaded wheel %s: x=%f, y=%f", 
                name.c_str(), config.x, config.y);
    
    wheel_configs_[name] = config;
  }

  // タイマーの設定 30hz
  timer_.setInterval(1000 / 30);
  timer_.start();
  connect(&timer_, &QTimer::timeout, this, &SwerveSteerWidget::onTimer);
}

void SwerveSteerWidget::onPushButtonClicked()
{
  static uint32_t counter = 0;
  // パブリッシャを使ってメッセージを送信
  std_msgs::msg::String msg;
  msg.data = std::to_string(++counter);
  qt_node_pub_handler_.publishMsg(msg);
}

void SwerveSteerWidget::onTimer()
{
  // サブスクライバを使ってメッセージを受信
  robot_interfaces::msg::WheelStates::SharedPtr msg;
  if (qt_node_sub_handler_.getLatestMsg(msg)) {
    current_wheel_states_ = msg;
  }

  update();
}

void SwerveSteerWidget::paintEvent(QPaintEvent * event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  
  // 背景を薄いグレーで塗りつぶし
  painter.fillRect(rect(), QColor(240, 240, 240));
  
  painter.setRenderHint(QPainter::Antialiasing);

  const double scale = 200.0; // 1m = 200px
  const double arrow_scale = 50.0; // 1m/s = 50px
  QPointF center(width() / 2.0, height() / 2.0);

  // ロボットの概形を描画（矩形）
  painter.setPen(QPen(Qt::black, 2));
  painter.drawRect(center.x() - 0.25 * scale, center.y() - 0.3 * scale, 0.5 * scale, 0.6 * scale);

  if (current_wheel_states_) {
    for (const auto & wheel : current_wheel_states_->wheels) {
      if (wheel_configs_.count(wheel.name)) {
        const auto & config = wheel_configs_.at(wheel.name);
        
        // ROS (X: forward, Y: left) -> Qt (X: right, Y: down)
        // ロボット正面を上にする
        double q_x = center.x() - config.y * scale;
        double q_y = center.y() - config.x * scale;
        QPointF wheel_pos(q_x, q_y);

        // ホイールの土台を描画
        painter.setPen(QPen(Qt::gray, 1));
        painter.setBrush(Qt::lightGray);
        painter.drawEllipse(wheel_pos, 10, 10);

        // 矢印を描画
        double angle = wheel.angle;
        double speed = wheel.speed;
        double length = speed * arrow_scale;

        // Qt座標系での方向
        double dx = -sin(angle) * length;
        double dy = -cos(angle) * length;
        QPointF end_pos(q_x + dx, q_y + dy);

        painter.setPen(QPen(Qt::red, 3));
        painter.drawLine(wheel_pos, end_pos);

        // 矢印の先端
        if (length > 5) {
          double head_len = 10;
          double head_angle = 0.5; // rad
          double a1 = angle + head_angle;
          double a2 = angle - head_angle;
          
          painter.drawLine(end_pos, end_pos + QPointF(sin(a1) * head_len, cos(a1) * head_len));
          painter.drawLine(end_pos, end_pos + QPointF(sin(a2) * head_len, cos(a2) * head_len));
        }
      }
    }
  }
}

} // namespace ui_swerve_steer

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(ui_swerve_steer::SwerveSteerWidget, rviz_common::Panel)
