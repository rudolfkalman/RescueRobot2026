#ifndef VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_
#define VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_

#include <memory>
#include <rviz_common/display.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <rclcpp/subscription.hpp>
#include <mutex>

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
  void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg);

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscription_;
  sensor_msgs::msg::Image::SharedPtr latest_image_;
  
  std::mutex image_mutex_;
};

} // namespace video_display_plugin

#endif  // VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_
