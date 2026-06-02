cmake_minimum_required(VERSION 3.5)
project(video_display_plugin)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake_auto REQUIRED)
find_package(rviz_common REQUIRED)
find_package(rviz_rendering REQUIRED)
find_package(pluginlib REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(OpenCV REQUIRED)

ament_auto_find_build_dependencies()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/video_display.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    include
)

target_link_libraries(${PROJECT_NAME}
  rviz_common::rviz_common
  rviz_rendering::rviz_rendering
  pluginlib::pluginlib
  ${OpenCV_LIBRARIES}
)

pluginlib_export_plugin_description_file(rviz_common plugins_description.xml)

ament_auto_package()
