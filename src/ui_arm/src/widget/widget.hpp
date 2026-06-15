#ifndef UI_ARM_WIDGET_HPP_
#define UI_ARM_WIDGET_HPP_

#include "ui_arm.h"

#ifndef Q_MOC_RUN
#include <rviz_common/panel.hpp>
#include <rviz_common/config.hpp>
#include <rviz_common/display_context.hpp>

#include "ui_arm/qt_node_pub_handler.hpp"
#include "robot_interfaces/msg/arm_states.hpp"
#endif

namespace ui_arm
{

class ArmWidget : public rviz_common::Panel
{
public:
  explicit ArmWidget(QWidget * parent = nullptr);

  void onInitialize() override;
  void load(const rviz_common::Config & config) override {}
  void save(rviz_common::Config config) const override {}

private:
  void publishAll();
  void resetAll();

  Ui::ArmWidget ui;
  QtNodePubHandler<robot_interfaces::msg::ArmStates> pub_handler_;
};

}  // namespace ui_arm
#endif  // UI_ARM_WIDGET_HPP_
