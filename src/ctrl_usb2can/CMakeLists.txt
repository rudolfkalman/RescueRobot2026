cmake_minimum_required(VERSION 3.10)
project(ctrl_usb2can C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_interfaces REQUIRED)

# Library - Changed to SHARED for ROS 2 standards
add_library(ctrl_usb2can SHARED
    src/lib_operator.c
)

target_include_directories(ctrl_usb2can PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

# Sample executable to test the library
add_executable(operator_example
    src/example.c
)

target_link_libraries(operator_example PRIVATE
    ctrl_usb2can
)

# Bridge Node
add_executable(usb2can_bridge src/usb2can_bridge.cpp)
target_link_libraries(usb2can_bridge ctrl_usb2can)
ament_target_dependencies(usb2can_bridge rclcpp robot_interfaces)

# Install
install(TARGETS ctrl_usb2can
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS operator_example usb2can_bridge
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY inc/
    DESTINATION include/${PROJECT_NAME}
)

# ament export
# This allows other packages to use find_package(ctrl_usb2can) 
# and link against ctrl_usb2can::ctrl_usb2can target
ament_export_include_directories(include/${PROJECT_NAME})
ament_export_libraries(ctrl_usb2can)
ament_export_targets(export_${PROJECT_NAME})

ament_package()
