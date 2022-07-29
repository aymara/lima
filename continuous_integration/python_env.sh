#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

export GCC_VERSION="10.1.0"
export LLVM_VERSION="14.0.6"

export BRANCH="public_analysis_content"
# QT_VERSION_PATCH >= 2
# For python > 3.10, QT_VERSION_PATCH >= 6
export QT_VERSION_MAJOR="5"
export QT_VERSION_MINOR="15"
export QT_VERSION_PATCH="2"
export QT_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
export QT_FULL_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
export PYSIDE_VERSION="2"
export PYTHON_VERSION="3.7"
# For python 3.7, it is cp37-cp37m
# For python 3.8, it is cp38-cp38
# For python 3.9, it is cp39-cp39
# For python 3.10, it is cp310-cp310
export PYTHON_SHORT_VERSION="cp37-cp37m"
export PYTHON_WHEEL_VERSION="cp37-abi3"
# For python 3.7, it is 3.7.13
# For python 3.8, it is 3.8.13
# For python 3.9, it is 3.9.13
# For python 3.10, it is 3.10.5
export PYTHON_FULL_VERSION="3.7.13"


# export GCC_VERSION="10.1.0"
# export LLVM_VERSION="14.0.6"
#
# export BRANCH="public_analysis_content"
# # QT_VERSION_PATCH >= 2
# # For python > 3.10, QT_VERSION_PATCH >= 6
# export QT_VERSION_MAJOR="5"
# export QT_VERSION_MINOR="15"
# export QT_VERSION_PATCH="2"
# export QT_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
# export QT_FULL_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
# export PYSIDE_VERSION="2"
# export PYTHON_VERSION="3.7"
# # For python 3.7, it is cp37-cp37m
# # For python 3.8, it is cp38-cp38
# # For python 3.9, it is cp39-cp39
# # For python 3.10, it is cp310-cp310
# export PYTHON_SHORT_VERSION="cp37-cp37m"
# export PYTHON_WHEEL_VERSION="cp37-abi3"
# # For python 3.7, it is 3.7.13
# # For python 3.8, it is 3.8.13
# # For python 3.9, it is 3.9.13
# # For python 3.10, it is 3.10.5
# export PYTHON_FULL_VERSION="3.7.13"
