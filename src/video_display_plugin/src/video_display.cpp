#include <rviz_common/window_manager_interface.hpp>
#include "video_display_plugin/video_display.hpp"
#include <pluginlib/class_list_macros.hpp>
#include <rviz_common/display_context.hpp>

namespace video_display_plugin
{

VideoDisplay::VideoDisplay()
: pipeline_(nullptr), appsink_(nullptr), running_(false), label_(nullptr)
{
  gst_init(nullptr, nullptr);
}

VideoDisplay::~VideoDisplay()
{
  stopPipeline();
}

void VideoDisplay::onInitialize()
{
  label_ = new QLabel(context_->getWindowManager()->getParentWindow());
  label_->setAlignment(Qt::AlignCenter);
  label_->setText("Waiting for video...");
  label_->resize(640, 480);
  label_->show();

  startPipeline();
}

void VideoDisplay::onEnable()
{
  startPipeline();
}

void VideoDisplay::onDisable()
{
  stopPipeline();
}

void VideoDisplay::update(float wall_dt, float ros_dt)
{
  Q_UNUSED(wall_dt);
  Q_UNUSED(ros_dt);

  std::lock_guard<std::mutex> lock(frame_mutex_);
  if (!latest_frame_.isNull() && label_) {
    label_->setPixmap(QPixmap::fromImage(latest_frame_));
    label_->show();
  }
}

void VideoDisplay::reset()
{
  std::lock_guard<std::mutex> lock(frame_mutex_);
  latest_frame_ = QImage();
}

void VideoDisplay::startPipeline()
{
  if (running_) return;

  // テスト用パイプライン（videotestsrc → BGR）
  pipeline_ = gst_parse_launch(
  "videotestsrc pattern=0 ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink",
  nullptr
  );

  if (!pipeline_) {
    RCLCPP_ERROR(rclcpp::get_logger("VideoDisplay"), "Failed to create pipeline");
    return;
  }

  appsink_ = gst_bin_get_by_name(GST_BIN(pipeline_), "sink");
  gst_app_sink_set_emit_signals(GST_APP_SINK(appsink_), false);
  gst_app_sink_set_drop(GST_APP_SINK(appsink_), true);
  gst_app_sink_set_max_buffers(GST_APP_SINK(appsink_), 1);

  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
  running_ = true;
  frame_thread_ = std::thread(&VideoDisplay::frameLoop, this);

  RCLCPP_INFO(rclcpp::get_logger("VideoDisplay"), "Pipeline started");
}

void VideoDisplay::stopPipeline()
{
  if (!running_) return;
  running_ = false;

  if (frame_thread_.joinable()) {
    frame_thread_.join();
  }

  if (pipeline_) {
    gst_element_set_state(pipeline_, GST_STATE_NULL);
    gst_object_unref(pipeline_);
    pipeline_ = nullptr;
    appsink_ = nullptr;
  }
}

void VideoDisplay::frameLoop()
{
  while (running_) {
    GstSample * sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink_), GST_SECOND);
    if (!sample) continue;

    GstBuffer * buffer = gst_sample_get_buffer(sample);
    GstCaps * caps = gst_sample_get_caps(sample);
    GstStructure * s = gst_caps_get_structure(caps, 0);

    int width = 0, height = 0;
    RCLCPP_INFO(rclcpp::get_logger("VideoDisplay"), "caps: %s", gst_caps_to_string(caps));
    gst_structure_get_int(s, "width", &width);
    gst_structure_get_int(s, "height", &height);

    RCLCPP_INFO(rclcpp::get_logger("VideoDisplay"), "Frame: %dx%d", width, height);

    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
  std::vector<uint8_t> buf(map.data, map.data + map.size);
  gst_buffer_unmap(buffer, &map);

  RCLCPP_INFO(rclcpp::get_logger("VideoDisplay"), "pixel[0]: R=%d G=%d B=%d", buf[0], buf[1], buf[2]);

  QImage img(buf.data(), width, height, width * 3, QImage::Format_RGB888);
  QImage copy = img.copy();

  {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    latest_frame_ = copy;
  }
}

    gst_sample_unref(sample);
  }
}

} // namespace video_display_plugin

PLUGINLIB_EXPORT_CLASS(video_display_plugin::VideoDisplay, rviz_common::Display)
