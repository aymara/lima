#!/bin/bash

set -o errexit
set -o pipefail

source setenv-lima.sh -m release
./gbuild.sh -m Release -n generic -d ON -p OFF
