#!/bin/bash
xhost +local:

docker compose up -d ros2

docker ps

docker exec -it ros2-jazzy /bin/bash
