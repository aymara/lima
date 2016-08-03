# Replace $PWD below by the path of  where you downloaded LIMA if you wish to 
# be able to source this script from elsewhere
export LIMA_ROOT=$PWD/..

LIMA_SOURCES=$PWD
pushd $LIMA_SOURCES
current_branch=`git rev-parse --abbrev-ref HEAD`
popd

# Path to the nltk data necessary to learn the English part of speech tagging 
# model.
export NLTK_PTB_DP_FILE=$HOME/nltk_data/corpora/dependency_treebank/nltk-ptb.dp

# Path to the LIMA linguistic data sources
export LINGUISTIC_DATA_ROOT=$LIMA_SOURCES/lima_linguisticdata

# Path to were the build will be done, where temporary binary files are written
export LIMA_BUILD_DIR=$LIMA_ROOT/Builds
install -d $LIMA_BUILD_DIR

# Path were LIMA will be installed
# replace debug below by release if you build LIMA in release mode (see 
# gbuild.sh) 
export LIMA_DIST=$LIMA_ROOT/Dist/$current_branch/debug
install -d $LIMA_DIST

# Path to where the LIMA configuration files are installed. Can be overriden at
# runtime
export LIMA_CONF=$LIMA_DIST/share/config/lima

# Path to where the LIMA runtime linguistic resources (compiled dictionary and 
# rules, ...) are installed. Can be overriden at runtime
export LIMA_RESOURCES=$LIMA_DIST/share/apps/lima/resources

# Where to install external  libraries if not using system ones
export LIMA_EXTERNALS=$LIMA_ROOT/externals

# Add the path to LIMA executables to the search path
export PATH=$LIMA_DIST/bin:$LIMA_EXTERNALS/bin:$PATH

# Add the path to LIMA libraries to the libaries search path
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib:$LD_LIBRARY_PATH

