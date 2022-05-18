#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set -o errexit
set -o pipefail
set -o nounset

docker build -f Dockerfile-manylinux_2_24 --build-arg CACHEBUST=$(date +%s) .  -t aymara/lima-manylinux_2_24:latest
docker push aymara/lima-manylinux_2_24:latest
