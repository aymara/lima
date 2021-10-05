#!/bin/bash

GITHUB_TOKEN="$1"

set -o errexit
set -o pipefail
set -o nounset


echo "Github API rate status: $(curl -i  https://api.github.com/repos/aymara/SVMTool/releases/latest?access_token=$GITHUB_TOKEN) "

SVMTOOL_LATEST_RELEASE_JSON=$(curl -s https://api.github.com/repos/aymara/SVMTool/releases/latest?access_token=$GITHUB_TOKEN)
echo "SVMTOOL_LATEST_RELEASE_JSON=$SVMTOOL_LATEST_RELEASE_JSON"

URL=$(echo "$SVMTOOL_LATEST_RELEASE_JSON" | grep browser_download_url |grep '.tgz"' | head -n 1 | cut -d '"' -f 4)
echo "URL=$URL"

curl  -vLJ -H 'Accept: application/octet-stream' $URL?access_token=$GITHUB_TOKEN | tar xvz

pushd SVMTool-1.3.1
perl Makefile.PL
make
make install
popd
