#/usr/bin/bash

cut -f1 $1 | sed -e 's/'\''s//' -e 's/s'\''//'

