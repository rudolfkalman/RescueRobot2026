import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node, PushRosNamespace
from launch.actions import GroupAction, DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():

    config_file = os.path.join(
        get_package_share_directory('robot_bringup'),
        'config',
        'robot_b.yaml'
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'joy_dev',
            default_value='/dev/input/js1',
            description='Joystick device path'
        ),

        GroupAction(
            actions=[
                PushRosNamespace('robot_b'),

                # USB2CAN Bridge
                Node(
                    package='ctrl_usb2can',
                    executable='usb2can_bridge',
                    name='usb2can_bridge',
                    parameters=[{'device_path': '/dev/usb2can_b'}],
                    output='screen'
                ),

                # Joy Node
                Node(
                    package="joy",
                    executable="joy_node",
                    name="joy_node",
                    parameters=[{'dev': LaunchConfiguration('joy_dev')}],
                ),

                # PS5 parser
                Node(
                    package="ctrl_ps5_joy_parser",
                    executable="ps5_joy_parser",
                    name="ps5_joy_parser",
                    output="screen",
                ),

                # Crawler Controller
                Node(
                  package='ctrl_crawler',
                  executable='crawler',
                  name='crawler_control',
                  parameters=[config_file]
                ),

                # RViz2
                Node(
                    package='rviz2',
                    executable='rviz2',
                    name='rviz2',
                    parameters=[config_file]
                ),
            ]
        )
    ])
