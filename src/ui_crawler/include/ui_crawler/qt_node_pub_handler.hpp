#ifndef UI_CRAWLER_QT_NODE_PUB_HANDLER_HPP_
#define UI_CRAWLER_QT_NODE_PUB_HANDLER_HPP_

#include <rclcpp/rclcpp.hpp>
#include "ros_types.hpp"

namespace ui_crawler
{

template<typename T>
class QtNodePubHandler
{
public:
  QtNodePubHandler() {}

  void setRosNodePtr(const rclcpp::Node::SharedPtr node_ptr)
  {
    node_ptr_ = node_ptr;
  }

  void initializePublisher(const std::string topic_name)
  {
    qt_node_publisher_ = node_ptr_->create_publisher<T>(topic_name, 10);
  }

  void finalizePublisher()
  {
    qt_node_publisher_.reset();
  }

  void publishMsg(const T & msg)
  {
    qt_node_publisher_->publish(msg);
  }

private:
  rclcpp::Node::SharedPtr node_ptr_;
  typename rclcpp::Publisher<T>::SharedPtr qt_node_publisher_;

};

} // namespace ui_crawler

#endif // UI_CRAWLER_QT_NODE_PUB_HANDLER_HPP_
