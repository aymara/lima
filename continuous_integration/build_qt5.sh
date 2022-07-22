#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# vim: tabstop=4 shiftwidth=4 softtabstop=4
# -*- sh-basic-offset: 4 -*-

set -exuo pipefail

BUILD_TARGET=/build
SRC=/src
QT_BRANCH="5.15.2"
MAKE_CORES="$(expr $(nproc) + 2)"

mkdir -p "$BUILD_TARGET"

echo "Building QT version $QT_BRANCH."

# This build process is inspired by
# https://www.tal.org/tutorials/building-qt-512-raspberry-pi
SRC_DIR="/src/qt-everywhere-src-5.15.2"

pushd $BUILD_TARGET
# @TODO: Add in the `-opengl es2` flag for Pi 1 - Pi 3.
# Currently this breaks the QTWebEngine process.
"${SRC_DIR}"/configure \
    "${BUILD_ARGS[@]}" \
    -platform "linux-g++" \
    -confirm-license \
    -dbus-linked \
    -no-eglfs \
    -no-evdev \
    -force-pkg-config \
    -no-glib \
    -make libs \
    -no-compile-examples \
    -no-cups \
    -no-gbm \
    -no-gtk \
    -no-pch \
    -no-use-gold-linker \
    -no-xcb \
    -no-xcb-xlib \
    -nomake examples \
    -nomake tests \
    -opensource \
    -prefix /opt/qt5 \
    -qpa eglfs \
    -qt-pcre \
    -reduce-exports \
    -release \
    -skip qt3d \
    -skip qtactiveqt \
    -skip qtandroidextras \
    -skip qtcanvas3d \
    -skip qtcharts \
    -skip qtdatavis3d \
    -skip qtgamepad \
    -skip qtgraphicaleffects \
    -skip qtlocation \
    -skip qtlottie \
    -skip qtmacextras \
    -skip qtpurchasing \
    -skip qtquick3d \
    -skip qtquickcontrols \
    -skip qtquickcontrols2 \
    -skip qtquicktimeline \
    -skip qtscript \
    -skip qtscxml \
    -skip qtsensors \
    -skip qtserialbus \
    -skip qtserialport \
    -skip qtspeech \
    -skip qttools \
    -skip qttranslations \
    -skip qtvirtualkeyboard \
    -skip qtwayland \
    -skip qtwebview \
    -skip qtwebengine \
    -skip qtwinextras \
    -skip qtx11extras \
    -skip wayland \
    -no-ssl \
    -system-freetype \
    -system-libjpeg \
    -system-libpng \
    -system-zlib

# The RAM consumption is proportional to the amount of cores.
# On an 8 core box, the build process will require ~16GB of RAM.
make -j"$MAKE_CORES"
make install

