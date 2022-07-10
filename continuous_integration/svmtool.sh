#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

GITHUB_TOKEN="$1"

set -o errexit
set -o pipefail
set -o nounset

# apt install dos2unix
wget --no-check-certificate https://osmot.cs.cornell.edu/svm_light/current/svm_light.tar.gz
install -d /svm_light
pushd /svm_light
tar xzf ../svm_light.tar.gz
dos2unix *.c
cat<<EOF > svm_light.patch
--- svm_hideo.c.s     2021-12-16 11:34:23.606959575 +0000
+++ svm_hideo.c       2021-12-16 11:34:33.614829980 +0000
@@ -31,7 +31,7 @@

 /* Common Block Declarations */

-long verbosity;
+extern long verbosity;

 # define PRIMAL_OPTIMAL      1

 # define DUAL_OPTIMAL        2

EOF
patch <svm_light.patch
make
cp svm_classify svm_learn /usr/bin
popd
rm -Rf /svm_light

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
