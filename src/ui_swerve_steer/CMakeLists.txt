cmake_minimum_required(VERSION 3.5)
project(ui_swerve_steer)

set(CMAKE_AUTOUIC ON)

find_package(ament_cmake_auto REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(robot_interfaces REQUIRED)

ament_auto_find_build_dependencies()

set(QT5_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)

include_directories(
    include
    src
)
ament_auto_add_library(swerve_steer SHARED
    src/widget/swerve_steer.ui
    src/widget/widget.cpp)
target_link_libraries(swerve_steer ${QT5_LIBS})

pluginlib_export_plugin_description_file(rviz_common plugins_description.xml)

ament_auto_package(USE_SCOPED_HEADER_INSTALL_DIR)
