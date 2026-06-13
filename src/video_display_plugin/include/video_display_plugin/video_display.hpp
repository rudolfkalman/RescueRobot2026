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
#include <rviz_common/panel.hpp>
#include <rviz_common/display_context.hpp>
#include <pluginlib/class_list_macros.hpp>

namespace video_display_plugin
{

class VideoDisplay : public rviz_common::Panel
{
  Q_OBJECT

public:
  explicit VideoDisplay(QWidget * parent = nullptr);
  ~VideoDisplay() override;

  void onInitialize() override;
  void load(const rviz_common::Config & config) override;
  void save(rviz_common::Config config) const override;

protected:
  void timerEvent(QTimerEvent * event) override;

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
  int timer_id_;
};

} // namespace video_display_plugin

#endif
