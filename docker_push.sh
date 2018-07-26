#!/bin/bash
echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
docker tag travis-ubuntu16.04 kleag/lima
docker push kleag/lima
