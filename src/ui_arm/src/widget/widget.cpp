#define _USE_MATH_DEFINES
#include "widget/widget.hpp"
#include <cmath>
#include <array>

namespace ui_arm
{

ArmWidget::ArmWidget(QWidget * parent)
: rviz_common::Panel(parent)
{
  ui.setupUi(this);
}

void ArmWidget::onInitialize()
{
  auto node = getDisplayContext()->getRosNodeAbstraction().lock()->get_raw_node();
  pub_handler_.setRosNodePtr(node);
  pub_handler_.initializePublisher("arm");

  conveyor_pub_handler_.setRosNodePtr(node);
  conveyor_pub_handler_.initializePublisher("wheels");

  struct JointEntry {
    QSlider * slider;
    QLabel  * label;
  };
  const std::array<JointEntry, 9> joints = {{
    {ui.slider_l_joint1, ui.val_l_joint1},
    {ui.slider_l_joint2, ui.val_l_joint2},
    {ui.slider_l_joint3, ui.val_l_joint3},
    {ui.slider_l_joint4, ui.val_l_joint4},
    {ui.slider_r_joint1, ui.val_r_joint1},
    {ui.slider_r_joint2, ui.val_r_joint2},
    {ui.slider_r_joint3, ui.val_r_joint3},
    {ui.slider_r_joint4, ui.val_r_joint4},
    {ui.slider_gripper,  ui.val_gripper},
  }};

  for (const auto & j : joints) {
    connect(j.slider, &QSlider::valueChanged, this,
      [this, lbl = j.label](int val) {
        lbl->setText(QString::number(val) + "°");
        publishAll();
      });
  }

  connect(ui.btn_conveyor_fwd,  &QPushButton::clicked, this, [this]{ publishConveyor( 1.0); });
  connect(ui.btn_conveyor_stop, &QPushButton::clicked, this, [this]{ publishConveyor( 0.0); });
  connect(ui.btn_conveyor_rev,  &QPushButton::clicked, this, [this]{ publishConveyor(-1.0); });

  connect(ui.btn_reset, &QPushButton::clicked, this, &ArmWidget::resetAll);
}

void ArmWidget::publishAll()
{
  struct JointDef {
    const char * name;
    QSlider    * slider;
  };
  const std::array<JointDef, 9> joints = {{
    {"l_joint1", ui.slider_l_joint1},
    {"l_joint2", ui.slider_l_joint2},
    {"l_joint3", ui.slider_l_joint3},
    {"l_joint4", ui.slider_l_joint4},
    {"r_joint1", ui.slider_r_joint1},
    {"r_joint2", ui.slider_r_joint2},
    {"r_joint3", ui.slider_r_joint3},
    {"r_joint4", ui.slider_r_joint4},
    {"gripper",  ui.slider_gripper},
  }};

  robot_interfaces::msg::ArmStates msg;
  for (const auto & j : joints) {
    robot_interfaces::msg::ArmState s;
    s.name  = j.name;
    // slider 0-180° → angle = (deg - 90) * π/180  [rad]
    s.angle = (j.slider->value() - 90.0) * M_PI / 180.0;
    msg.joints.push_back(s);
  }
  pub_handler_.publishMsg(msg);
}

void ArmWidget::publishConveyor(double speed)
{
  robot_interfaces::msg::WheelStates msg;
  robot_interfaces::msg::WheelState s;
  s.name  = "conveyor";
  s.speed = speed;
  s.angle = 0.0;
  msg.wheels.push_back(s);
  conveyor_pub_handler_.publishMsg(msg);
}

void ArmWidget::resetAll()
{
  const std::array<QSlider *, 9> sliders = {{
    ui.slider_l_joint1, ui.slider_l_joint2,
    ui.slider_l_joint3, ui.slider_l_joint4,
    ui.slider_r_joint1, ui.slider_r_joint2,
    ui.slider_r_joint3, ui.slider_r_joint4,
    ui.slider_gripper,
  }};
  for (QSlider * s : sliders) {
    s->setValue(90);
  }
}

}  // namespace ui_arm

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(ui_arm::ArmWidget, rviz_common::Panel)
