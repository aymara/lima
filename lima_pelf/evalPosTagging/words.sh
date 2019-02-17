#/bin/bash

set -o nounset
set -o errexit
set -o pipefail

cut -f1 $1 | sed -e 's/'\''s//' -e 's/s'\''//'

