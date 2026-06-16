#ifndef UI_ARM_QT_NODE_PUB_HANDLER_HPP_
#define UI_ARM_QT_NODE_PUB_HANDLER_HPP_

#include <rclcpp/rclcpp.hpp>

namespace ui_arm
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

  void initializePublisher(const std::string & topic_name)
  {
    publisher_ = node_ptr_->create_publisher<T>(topic_name, 10);
  }

  void publishMsg(const T & msg)
  {
    if (publisher_) {
      publisher_->publish(msg);
    }
  }

private:
  rclcpp::Node::SharedPtr node_ptr_;
  typename rclcpp::Publisher<T>::SharedPtr publisher_;
};

}  // namespace ui_arm
#endif  // UI_ARM_QT_NODE_PUB_HANDLER_HPP_
