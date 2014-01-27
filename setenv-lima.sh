export LIMA_ROOT=$PWD/..
LIMA_SOURCES=$LIMA_ROOT/Sources
pushd $LIMA_SOURCES
current_branch=`git rev-parse --abbrev-ref HEAD`
popd

export LINGUISTIC_DATA_ROOT=$LIMA_ROOT/Sources/lima_linguisticData

export LIMA_BUILD_DIR=$LIMA_ROOT/Builds
install -d $LIMA_BUILD_DIR
export LIMA_DIST=$LIMA_ROOT/Dist/$current_branch/debug
install -d $LIMA_DIST
export LIMA_DIST_ROOT=$LIMA_DIST

export LIMA_CONF=$LIMA_DIST/share/config/lima
export LIMA_RESOURCES=$LIMA_DIST_ROOT/share/apps/lima/resources
export LIMA_EXTERNALS=$LIMA_ROOT/externals

export PATH=$LIMA_DIST/bin:$LIMA_EXTERNALS/bin:$PATH
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib


