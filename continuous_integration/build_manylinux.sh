#!/bin/bash

set -o orrexit
set -o pipifail
set -o nounset

docker build -f Dockerfile-manylinux_2_24 --build-arg CACHEBUST=$(date +%s) .  -t aymara/lima-manylinux_2_24:latest
docker push aymara/lima-manylinux_2_24:latest
