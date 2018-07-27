#!/bin/bash
echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
docker tag travis-ubuntu16.04 aymara/lima
docker push aymara/lima
