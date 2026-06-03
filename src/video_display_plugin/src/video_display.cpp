#include "video_display_plugin/video_display.hpp"
#include <pluginlib/class_list_macros.hpp>
#include <rviz_common/display_context.hpp>

namespace video_display_plugin
{

VideoDisplay::VideoDisplay()
: rviz_common::Display()
{
}

VideoDisplay::~VideoDisplay() = default;

void VideoDisplay::onInitialize()
{
  // ROS ノードを取得
  auto context = getDisplayContext();
  if (!context) {
    RCLCPP_ERROR(rclcpp::get_logger("VideoDisplay"), "Display context is null");
    return;
  }
  
  auto ros_node_abs = context->getRosNodeAbstraction().lock();
  if (!ros_node_abs) {
    RCLCPP_ERROR(rclcpp::get_logger("VideoDisplay"), "ROS node abstraction is null");
    return;
  }
  
  ros_node_ = ros_node_abs->get_raw_node();
  if (!ros_node_) {
    RCLCPP_ERROR(rclcpp::get_logger("VideoDisplay"), "Raw ROS node is null");
    return;
  }

  // 画像トピックをサブスクライブ
  image_subscription_ = ros_node_->create_subscription<sensor_msgs::msg::Image>(
    "/camera/image_raw",
    10,
    std::bind(&VideoDisplay::imageCallback, this, std::placeholders::_1));
  
  RCLCPP_INFO(ros_node_->get_logger(), "VideoDisplay initialized");
}

void VideoDisplay::onEnable()
{
}

void VideoDisplay::onDisable()
{
}

void VideoDisplay::update(float wall_dt, float ros_dt)
{
  Q_UNUSED(wall_dt);
  Q_UNUSED(ros_dt);
  
  // 画像キューを処理
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (!image_queue_.empty()) {
      auto latest_image = image_queue_.front();
      image_queue_.pop();
      // TODO: ここで画像をレンダリング
    }
  }
}

void VideoDisplay::reset()
{
  // キューをクリア
  std::lock_guard<std::mutex> lock(queue_mutex_);
  while (!image_queue_.empty()) {
    image_queue_.pop();
  }
}

void VideoDisplay::imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
{
  std::lock_guard<std::mutex> lock(queue_mutex_);
  image_queue_.push(msg);
  
  // キューサイズを制限（最新1枚のみ保持）
  if (image_queue_.size() > 1) {
    image_queue_.pop();
  }
}

} // namespace video_display_plugin

PLUGINLIB_EXPORT_CLASS(video_display_plugin::VideoDisplay, rviz_common::Display)