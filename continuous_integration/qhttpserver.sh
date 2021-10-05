#!/bin/bash

GITHUB_TOKEN="$1"

set -o errexit
set -o pipefail
set -o nounset


echo "Github API rate status: $(curl -i  https://api.github.com/repos/aymara/qhttpserver/releases/latest?access_token=$GITHUB_TOKEN) "

QHTTPSERVER_LATEST_RELEASE_JSON=$(curl -s https://api.github.com/repos/aymara/qhttpserver/releases/latest?access_token=$GITHUB_TOKEN)
echo "QHTTPSERVER_LATEST_RELEASE_JSON=$QHTTPSERVER_LATEST_RELEASE_JSON"

URL=$(echo "$QHTTPSERVER_LATEST_RELEASE_JSON"| grep browser_download_url | grep debian9 | grep '.deb"' | head -n 1 | cut -d '"' -f 4)
echo "URL=$URL"
TEMP_DEB="$(mktemp)"
curl  -vLJ -H 'Accept: application/octet-stream' $URL?access_token=$GITHUB_TOKEN -o "$TEMP_DEB"
dpkg -i "$TEMP_DEB"

