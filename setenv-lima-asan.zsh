#####!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

invoked=$_   # needs to be first thing in the script
OPTIND=1    # also remember to initialize your flags and other variables

source setenv-lima.zsh -m asan
