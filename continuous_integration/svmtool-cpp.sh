#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

GITHUB_TOKEN=$1

echo "Github API rate status: $(curl -i  https://api.github.com/repos/aymara/svmtool-cpp/releases/latest?access_token=$GITHUB_TOKEN) "

SVMTOOL_LATEST_RELEASE_JSON=$(curl -s https://api.github.com/repos/aymara/svmtool-cpp/releases/latest?access_token=$GITHUB_TOKEN) 
echo "SVMTOOL_LATEST_RELEASE_JSON=$SVMTOOL_LATEST_RELEASE_JSON" 

URL=$(echo "$SVMTOOL_LATEST_RELEASE_JSON" | grep browser_download_url | grep debian9 | grep '.deb"' | head -n 1 | cut -d '"' -f 4) 
echo "URL=$URL"  

TEMP_DEB="$(mktemp)" 
curl  -vLJ -H 'Accept: application/octet-stream' $URL?access_token=$GITHUB_TOKEN -o "$TEMP_DEB" 
dpkg -i "$TEMP_DEB"
