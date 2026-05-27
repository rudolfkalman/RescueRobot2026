cmake_minimum_required(VERSION 3.8)
project(ctrl_swerve_steer)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(robot_interfaces REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

add_executable(swerve_steer src/swerve_steer.cpp)

target_link_libraries (swerve_steer Eigen3::Eigen)

ament_target_dependencies(swerve_steer 
  rclcpp 
  std_msgs 
  sensor_msgs 
  geometry_msgs
  robot_interfaces 
)

install(TARGETS
  swerve_steer
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
