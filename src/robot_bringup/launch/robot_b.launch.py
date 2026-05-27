import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    config_file = os.path.join(
        get_package_share_directory('robot_bringup'),
        'config',
        'robot_b.yaml'
    )

    return LaunchDescription([
        # Joy Node
        Node(
            package="joy",
            executable="joy_node",
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

    ])
