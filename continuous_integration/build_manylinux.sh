#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set -o errexit
set -o pipefail
set -o nounset

export PYTHON_VERSION=3.7
export PYTHON_SHORT_VERSION=37
docker build --progress=plain -f Dockerfile-manylinux_2_24 --build-arg PYTHON_VERSION=${PYTHON_VERSION} --build-arg PYTHON_SHORT_VERSION=${PYTHON_SHORT_VERSION} --build-arg CACHEBUST=$(date +%s) .  -t aymara/lima-manylinux_2_24:latest
# docker push aymara/lima-manylinux_2_24-python${PYTHON_VERSION}:latest
