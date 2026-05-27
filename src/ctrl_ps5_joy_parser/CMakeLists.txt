cmake_minimum_required(VERSION 3.8)
project(ctrl_ps5_joy_parser)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(robot_interfaces REQUIRED)

add_executable(ps5_joy_parser src/ps5_joy_parser.cpp)

ament_target_dependencies(ps5_joy_parser
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  robot_interfaces
)

install(TARGETS
  ps5_joy_parser
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
