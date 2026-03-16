#!/bin/bash

xhost +local:

docker compose down

docker compose up -d

docker ps

docker exec -it ros2-jazzy /bin/bash
