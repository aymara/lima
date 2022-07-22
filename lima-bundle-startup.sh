#!/bin/sh
#
# Author: Aaron Voisine <aaron@voisine.org>
# Inkscape Modifications: Michael Wybrow <mjwybrow@users.sourceforge.net>
# K-3D Modifications: Timothy M. Shead <tshead@k-3d.com>
# LIMA Modifications: Gael de Chalendar <gael.de-chalendar@cea.fr>

echo $0

LIMA_BUNDLE=/Volumes/lima-2.1.-Darwin/lima-2.1.-Darwin.app
LIMA_BUNDLE_RESOURCES="$LIMA_BUNDLE/Contents/Resources"

echo "LIMA_BUNDLE: $LIMA_BUNDLE"
echo "LIMA_BUNDLE_RESOURCES: $LIMA_BUNDLE_RESOURCES"

export "DYLD_LIBRARY_PATH=$LIMA_BUNDLE_RESOURCES/lib"
export "PATH=$LIMA_BUNDLE_RESOURCES/bin:$PATH"
export "LIMA_CONF=$LIMA_BUNDLE_RESOURCES/share/config/lima"
echo "LIMA_CONF: $LIMA_CONF"
export "LIMA_RESOURCES=$LIMA_BUNDLE_RESOURCES/share/apps/lima/resources"
echo "LIMA_RESOURCES: $LIMA_RESOURCES"

#export
exec "$LIMA_BUNDLE_RESOURCES/bin/lima"
