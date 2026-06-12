#ifndef VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_
#define VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_

#include <memory>
#include <mutex>
#include <atomic>
#include <thread>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <QImage>
#include <QLabel>

#include <rclcpp/rclcpp.hpp>
#include <rviz_common/display.hpp>
#include <rviz_common/display_context.hpp>
#include <pluginlib/class_list_macros.hpp>

namespace video_display_plugin
{

class VideoDisplay : public rviz_common::Display
{
  Q_OBJECT

public:
  VideoDisplay();
  ~VideoDisplay() override;

  void onInitialize() override;
  void onEnable() override;
  void onDisable() override;
  void update(float wall_dt, float ros_dt) override;
  void reset() override;

private:
  void startPipeline();
  void stopPipeline();
  void frameLoop();

  GstElement * pipeline_;
  GstElement * appsink_;

  std::atomic<bool> running_;
  std::thread frame_thread_;

  std::mutex frame_mutex_;
  QImage latest_frame_;

  QLabel * label_;
};

} // namespace video_display_plugin

#endif
