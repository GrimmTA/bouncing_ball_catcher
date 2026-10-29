import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('bouncing_ball_catcher'),
        'config', 'params.yaml')

    return LaunchDescription([
        Node(
            package='bouncing_ball_catcher',
            executable='camera_node',
            name='camera_node',
            parameters=[config],
            output='screen',
        ),
        Node(
            package='bouncing_ball_catcher',
            executable='game_node',
            name='game_node',
            parameters=[config],
            output='screen',
        ),
    ])