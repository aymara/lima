#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set -o errexit
set -o pipefail
set -o nounset

GITHUB_TOKEN="$1"
DISTRIB="$2"

QHTTPSERVER_LATEST_RELEASE_JSON=$(curl -s https://api.github.com/repos/aymara/qhttpserver/releases/latest)
echo "QHTTPSERVER_LATEST_RELEASE_JSON=$QHTTPSERVER_LATEST_RELEASE_JSON"

URL=$(echo "$QHTTPSERVER_LATEST_RELEASE_JSON"| grep browser_download_url | grep "${DISTRIB}" | grep '.deb"' | head -n 1 | cut -d '"' -f 4)
echo "URL=$URL"
TEMP_DEB_DIR="$(mktemp -d)"
curl  -LJ -H 'Accept: application/octet-stream' -H "Authorization: token ${GITHUB_TOKEN}" $URL --output-dir "$TEMP_DEB_DIR" --output release.tar
pushd "$TEMP_DEB_DIR"
tar xf ./release.tar
dpkg -i *.deb
popd
