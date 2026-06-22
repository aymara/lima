#!/bin/bash

set -o errexit
set -o pipefail

source setenv-lima.sh -m asan
./gbuild.sh -m Debug -n generic -d ON -p OFF -a ON
