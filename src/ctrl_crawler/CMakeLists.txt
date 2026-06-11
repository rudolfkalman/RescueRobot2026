cmake_minimum_required(VERSION 3.8)
project(ctrl_crawler)

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

add_executable(crawler src/crawler.cpp)

target_link_libraries (crawler 
  Eigen3::Eigen
)

ament_target_dependencies(crawler 
  rclcpp 
  std_msgs 
  sensor_msgs 
  geometry_msgs
  robot_interfaces 
)

install(TARGETS
  crawler
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
