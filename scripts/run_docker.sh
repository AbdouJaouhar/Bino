#!/bin/bash
xhost +local:docker
docker compose -f docker/docker-compose.yaml down
docker compose -f docker/docker-compose.yaml up --build

#cd /workspace && rm -rf build && mkdir -p build && cd build && cmake -G Ninja .. && ninja && ./bino_app 640 480 0 30 CAM_A 300 300 0 30 CAM_A 300 300 1 30 CAM_A
