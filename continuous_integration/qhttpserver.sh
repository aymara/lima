#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Install the qhttpserver package for the given distribution from a pinned
# aymara/qhttpserver release. The version is pinned (no GitHub API
# "releases/latest" query) so that builds are reproducible and not subject to
# API rate limits; the asset is checked against a known sha256.
#
# Usage: qhttpserver.sh DISTRIB   (debian12 | ubuntu22.04)
#
# To upgrade: change RELEASE_TAG/PKG_VERSION and update the checksums below.

set -o errexit
set -o pipefail
set -o nounset

DISTRIB="$1"

RELEASE_TAG="1.1.2"
PKG_VERSION="1.1.1"

case "${DISTRIB}" in
  debian12)    SHA256="4fa3fa0f27facbf109936f54a9af34bb52600f57588d7af22e341c04d24a5dab" ;;
  ubuntu22.04) SHA256="eb3cb3ccb192a54b449bdcc971ae3d0ded70948fd08bbb57a0e819bcae27849c" ;;
  *) echo "qhttpserver.sh: no pinned qhttpserver package for '${DISTRIB}'" >&2; exit 1 ;;
esac

# The release assets are named .deb but are tar archives containing the .deb.
URL="https://github.com/aymara/qhttpserver/releases/download/${RELEASE_TAG}/qhttpserver-${PKG_VERSION}-${DISTRIB}.deb"
echo "URL=$URL"
TEMP_DEB_DIR="$(mktemp -d)"
curl -fsSL --retry 3 "$URL" --output "$TEMP_DEB_DIR/release.tar"
echo "${SHA256}  $TEMP_DEB_DIR/release.tar" | sha256sum -c -
pushd "$TEMP_DEB_DIR"
tar xf ./release.tar
dpkg -i *.deb
popd
rm -rf "$TEMP_DEB_DIR"
