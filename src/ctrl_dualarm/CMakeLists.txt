cmake_minimum_required(VERSION 3.10)
project(ctrl_dualarm C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_interfaces REQUIRED)

# Serial protocol library (C). Static so it is baked into the node.
add_library(lib_servo STATIC
  src/lib_servo.c
)
target_include_directories(lib_servo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
set_target_properties(lib_servo PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Bridge node
add_executable(dualarm_bridge src/dualarm_bridge.cpp)
target_link_libraries(dualarm_bridge lib_servo)
ament_target_dependencies(dualarm_bridge rclcpp robot_interfaces)

install(TARGETS dualarm_bridge
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY inc/
  DESTINATION include/${PROJECT_NAME}
)

ament_package()
