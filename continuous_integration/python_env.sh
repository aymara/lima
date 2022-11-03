#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

export MANYLINUX_TAG="2022-10-25-fbea779"

export GCC_VERSION="10.1.0"
export LLVM_VERSION="14.0.6"

export BRANCH="public_analysis_content"
export QT_VERSION_MAJOR="5"
export QT_VERSION_MINOR="15"
export QT_VERSION_PATCH="2"
export QT_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
export QT_FULL_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
export PYSIDE_VERSION="2"
export PYTHON_VERSION="3.7"
export PYTHON_SHORT_VERSION="cp37-cp37m"
export PYTHON_WHEEL_VERSION="cp37-abi3"
export PYTHON_FULL_VERSION="3.7.15"

