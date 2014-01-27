##!bin/sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
DICTIONNARY_PATH=$RESOURCE_PATH/build/analysisDictionary/fre

echo "Compiling and installing dictionnary"
pushd $DICTIONNARY_PATH
make && make install
popd
