#!/bin/bash
xhost +local:docker
docker compose -f docker/docker-compose.yaml up --build
