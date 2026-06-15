import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node, PushRosNamespace
from launch.actions import GroupAction

def generate_launch_description():

    config_file = os.path.join(
        get_package_share_directory('robot_bringup'),
        'config',
        'robot_b.yaml'
    )

    return LaunchDescription([
        GroupAction(
            actions=[
                PushRosNamespace('robot_b'),

                # 8-axis arm bridge (esp_arm8)
                Node(
                    package='ctrl_dualarm',
                    executable='dualarm_bridge',
                    name='dualarm_bridge',
                    parameters=[config_file],
                    output='screen'
                ),

                # 1-axis gripper bridge (esp_arm1)
                Node(
                    package='ctrl_dualarm',
                    executable='dualarm_bridge',
                    name='gripper_bridge',
                    parameters=[config_file],
                    output='screen'
                ),

                # RViz2
                Node(
                    package='rviz2',
                    executable='rviz2',
                    name='rviz2',
                ),
            ]
        )
    ])
