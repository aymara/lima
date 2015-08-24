# Replace $PWD below by the path of  where you downloaded LIMA if you wish to 
# be able to source this script from elsewhere
export LIMA_ROOT=$PWD/..

LIMA_SOURCES=$PWD
pushd $LIMA_SOURCES
current_branch=`git rev-parse --abbrev-ref HEAD`
popd

export NLTK_PTB_DP_FILE=$HOME/nltk_data/corpora/dependency_treebank/nltk-ptb.dp
export LINGUISTIC_DATA_ROOT=$LIMA_SOURCES/lima_linguisticData

export LIMA_BUILD_DIR=$LIMA_ROOT/Builds
install -d $LIMA_BUILD_DIR

# replace debug below by release if you build LIMA in release mode (see 
# gbuild.sh) 
export LIMA_DIST=$LIMA_ROOT/Dist/$current_branch/debug
install -d $LIMA_DIST

export LIMA_CONF=$LIMA_DIST/share/config/lima
export LIMA_RESOURCES=$LIMA_DIST/share/apps/lima/resources
export LIMA_EXTERNALS=$LIMA_ROOT/externals

export PATH=$LIMA_DIST/bin:$LIMA_EXTERNALS/bin:$PATH
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib:$LD_LIBRARY_PATH
