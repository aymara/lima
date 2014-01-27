##!bin/sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
IDIOMS_PATH=$RESOURCE_PATH/build/rules-idiom/fre

echo "Compiling and installing idiomatic expressions"
pushd $IDIOMS_PATH
make && make install
popd
