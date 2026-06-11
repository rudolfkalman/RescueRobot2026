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

namespace ui_crawler
{

  CrawlerWidget::CrawlerWidget(QWidget * parent)
: rviz_common::Panel(parent)
{
  // UIの初期化
  ui.setupUi(this);
}

void CrawlerWidget::onInitialize()
{
  auto node = this->getDisplayContext()->getRosNodeAbstraction().lock()->get_raw_node();

  // パブリッシャの初期化
  qt_node_pub_handler_.setRosNodePtr(node);
  qt_node_pub_handler_.initializePublisher("crawler_ui_topic");

  // サブスクライバの初期化
  qt_node_sub_handler_.setRosNodePtr(node);
  qt_node_sub_handler_.initializeSubscription("wheels");

  // パラメータのロード (2輪クローラー用)
  try {
    if (!node->has_parameter("tracks.left.x")) {
      node->declare_parameter("tracks.left.x", -0.2);
    }
    if (!node->has_parameter("tracks.right.x")) {
      node->declare_parameter("tracks.right.x", 0.2);
    }
  } catch (const rclcpp::exceptions::ParameterAlreadyDeclaredException & e) {}

  node->get_parameter("tracks.left.x", left_track_x_);
  node->get_parameter("tracks.right.x", right_track_x_);
  
  RCLCPP_INFO(node->get_logger(), "UI: Loaded tracks: left_x=%f, right_x=%f", 
              left_track_x_, right_track_x_);

  // タイマーの設定 30hz
  timer_.setInterval(1000 / 30);
  timer_.start();
  connect(&timer_, &QTimer::timeout, this, &CrawlerWidget::onTimer);
}

void CrawlerWidget::onPushButtonClicked()
{
  static uint32_t counter = 0;
  std_msgs::msg::String msg;
  msg.data = std::to_string(++counter);
  qt_node_pub_handler_.publishMsg(msg);
}

void CrawlerWidget::onTimer()
{
  robot_interfaces::msg::WheelStates::SharedPtr msg;
  if (qt_node_sub_handler_.getLatestMsg(msg)) {
    current_wheel_states_ = msg;
  }

  update();
}

void CrawlerWidget::paintEvent(QPaintEvent * event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  
  painter.fillRect(rect(), QColor(240, 240, 240));
  painter.setRenderHint(QPainter::Antialiasing);

  const double scale = 200.0; // 1m = 200px
  const double arrow_scale = 50.0; // 1m/s = 50px
  QPointF center(width() / 2.0, height() / 2.0);

  // ロボットの概形を描画
  painter.setPen(QPen(Qt::black, 2));
  painter.drawRect(center.x() - 0.25 * scale, center.y() - 0.3 * scale, 0.5 * scale, 0.6 * scale);

  if (current_wheel_states_) {
    for (const auto & track_msg : current_wheel_states_->wheels) {
      double track_x = 0.0;
      if (track_msg.name == "left") {
        track_x = left_track_x_;
      } else if (track_msg.name == "right") {
        track_x = right_track_x_;
      } else {
        continue;
      }

      // ROS (X: forward, Y: left) -> Qt (X: right, Y: down)
      // 横方向オフセットはROSのYだが、ここではtrack_xとして左右のズレを表現
      // (track_xが負なら左、正なら右とする)
      double q_x = center.x() + track_x * scale;
      double q_y = center.y(); // 前後位置はセンター固定とする
      QPointF track_pos(q_x, q_y);

      // 履帯を描画
      painter.setPen(QPen(Qt::gray, 1));
      painter.setBrush(Qt::lightGray);
      painter.drawRect(q_x - 10, q_y - 40, 20, 80);

      // 矢印を描画
      double angle = track_msg.angle;
      double speed = track_msg.speed;
      double length = speed * arrow_scale;

      double dx = -sin(angle) * length;
      double dy = -cos(angle) * length;
      QPointF end_pos(q_x + dx, q_y + dy);

      painter.setPen(QPen(Qt::red, 3));
      painter.drawLine(track_pos, end_pos);

      if (std::abs(length) > 5) {
        double head_len = 10;
        double head_angle = 0.5;
        double a = (speed > 0) ? angle : (angle + M_PI);
        
        painter.drawLine(end_pos, end_pos + QPointF(sin(a + head_angle) * head_len, cos(a + head_angle) * head_len));
        painter.drawLine(end_pos, end_pos + QPointF(sin(a - head_angle) * head_len, cos(a - head_angle) * head_len));
      }
    }
  }
}

} // namespace ui_crawler

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(ui_crawler::CrawlerWidget, rviz_common::Panel)
