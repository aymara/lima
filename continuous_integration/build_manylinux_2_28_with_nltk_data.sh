#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set -o errexit
set -o pipefail
set -o nounset

docker build --progress=plain -f Dockerfile-manylinux_2_28_with_nltk_data -t aymara/manylinux_2_28_with_nltk_data:latest .
docker push aymara/manylinux_2_28_with_nltk_data:latest
