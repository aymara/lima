# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

rm deeplima.tar
docker build -t deeplima -f build_scripts/Dockerfile . && docker save -o deeplima.tar deeplima

