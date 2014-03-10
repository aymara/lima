##!bin/sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
NGRAMS_PATH=$RESOURCE_PATH/build/disambiguisationMatrices/fre

echo "Compiling and installing disambiguisation matrices (ngrams)"
pushd $NGRAMS_PATH
make && make install
popd