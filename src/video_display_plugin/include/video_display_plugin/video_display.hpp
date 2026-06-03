#ifndef VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_
#define VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_

#include <memory>
#include <rviz_common/display.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <rclcpp/subscription.hpp>
#include <mutex>
#include <queue>

namespace video_display_plugin
{

class VideoDisplay : public rviz_common::Display
{
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

  rclcpp::Node::SharedPtr ros_node_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscription_;
  std::queue<sensor_msgs::msg::Image::SharedPtr> image_queue_;
  
  std::mutex queue_mutex_;
};

} // namespace video_display_plugin

#endif  // VIDEO_DISPLAY_PLUGIN__VIDEO_DISPLAY_HPP_