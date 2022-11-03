#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

export MANYLINUX_TAG="2022-10-25-fbea779"
export GCC_VERSION="11.2.1"
export LLVM_VERSION="13.0.1"

export BRANCH="port-to-qt6"
export QT_VERSION_MAJOR="6"
export QT_VERSION_MINOR="4"
export QT_VERSION_PATCH="0"
export QT_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
export QT_FULL_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
export PYSIDE_VERSION="6"
export PYTHON_VERSION="3.7"
export PYTHON_SHORT_VERSION="cp37-cp37m"
export PYTHON_WHEEL_VERSION="cp37-abi3"
export PYTHON_FULL_VERSION="3.7.15"


