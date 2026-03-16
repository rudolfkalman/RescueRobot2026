#/bin/bash

SERVICE_NAME="ros2"
CONTAINER_NAME="ros2-jazzy"

if [ "$(docker compose ps -q ${SERVICE_NAME})" ]; then
  echo "Entering ${CONTAINER_NAME}."
  docker exec -it ${CONTAINER_NAME} /bin/bash
else
  echo "${CONTAINER_NAME} is not running."
  exit 1
fi
