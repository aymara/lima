#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

GITHUB_TOKEN="$1"

set -o errexit
set -o pipefail
set -o nounset

echo "Github API rate status: $(curl -i -H 'Authorization: token ${GITHUB_TOKEN}' https://api.github.com/repos/aymara/svmtool-cpp/releases/latest)"

SVMTOOL_LATEST_RELEASE_JSON=$(curl -s -H "Authorization: token ${GITHUB_TOKEN}" https://api.github.com/repos/aymara/svmtool-cpp/releases/latest)
echo "SVMTOOL_LATEST_RELEASE_JSON=$SVMTOOL_LATEST_RELEASE_JSON"

URL=$(echo "$SVMTOOL_LATEST_RELEASE_JSON" | grep browser_download_url | grep debian9 | grep '.deb"' | head -n 1 | cut -d '"' -f 4)
echo "URL=$URL"

TEMP_DEB="$(mktemp)"
curl  -LJ -H 'Accept: application/octet-stream' -H "Authorization: token ${GITHUB_TOKEN}" $URL -o "$TEMP_DEB"
dpkg -i "$TEMP_DEB"
