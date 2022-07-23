#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set -o errexit
set -o pipefail
set -o nounset

docker build -f Dockerfile-manylinux_2_24_with_llvm -t aymara/manylinux_2_24_with_llvm:latest .
docker push aymara/manylinux_2_24_with_llvm:latest