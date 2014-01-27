##!bin/sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
SYNRULES_PATH=$RESOURCE_PATH/build/syntacticAnalysis/fre

echo "Compiling and installing syntactic rules"
pushd $SYNRULES_PATH
make && make install
popd
