/*
 * Copyright (c) 2024 NITK.K ROS-Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CRAWLER_WIDGET_HPP_
#define CRAWLER_WIDGET_HPP_

// 作成した.uiファイル名によって変わる
// xxx.ui -> ui_xxx.h
#include "ui_crawler.h"
#include <QTimer>
#include <QPaintEvent>

// Q_MOC_RUN is defined when this file is processed by moc
#ifndef Q_MOC_RUN
#include <rviz_common/panel.hpp>

#include <rviz_common/config.hpp>
#include <rviz_common/display_context.hpp>

#include "ui_crawler/qt_node_pub_handler.hpp"
#include "ui_crawler/qt_node_sub_handler.hpp"
#endif

namespace ui_crawler
{

// Widgetのクラス
class CrawlerWidget : public rviz_common::Panel
{
public:
  // 初期化
  explicit CrawlerWidget(QWidget * parent = nullptr);

public:
// rviz_commonのAPI : デフォルトの実装は空なので、必要に応じてオーバーライドする
  void onInitialize() override;
  void load(const rviz_common::Config & config) override {}
  void save(rviz_common::Config config) const override {}

private:
  // ボタンがクリックされたときの処理
  void onPushButtonClicked();
  void onTimer();

protected:
  void paintEvent(QPaintEvent * event) override;

private:
  QTimer timer_;
  Ui::CrawlerWidget ui;

  // パブリッシャのハンドラ
  QtNodePubHandler<std_msgs::msg::String> qt_node_pub_handler_;
  QtNodeSubHandler<robot_interfaces::msg::WheelStates> qt_node_sub_handler_;

  double left_track_x_ = -0.2;
  double right_track_x_ = 0.2;
  robot_interfaces::msg::WheelStates::SharedPtr current_wheel_states_;
};

} // namespace ui_crawler

#endif //CRAWLER_WIDGET_HPP_
