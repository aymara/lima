##!bin/sh

# ssh and rsync configuration
source $LEFFE_COMMANDS_ROOT/sshcommands.sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
IDIOMS_PATH=$RESOURCE_PATH/rules-idiom/fre
REMOTE_IDIOMS_PATH=/home/leffe/Lima/Sources/linguisticData/rules-idiom/fre

echo "Syncing source files"
rsyncexec $IDIOMS_PATH/src/idioms-fre.txt leffe@calamondin:$REMOTE_IDIOMS_PATH/src/idioms-fre.txt

echo "Remote installation"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/install-idioms.sh &

# local installation
echo "Compiling and installing idiomatic expressions"
pushd $IDIOMS_PATH
make clean && make && make install
popd

