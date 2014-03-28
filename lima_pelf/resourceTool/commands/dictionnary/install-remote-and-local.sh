##!bin/sh

# ssh and rsync configuration
source $PELF_COMMANDS_ROOT/sshcommands.sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
DICTIONNARY_PATH=$RESOURCE_PATH/analysisDictionary
REMOTE_DICTIONNARY_PATH=/home/leffe/Lima/Sources/linguisticData/analysisDictionary

echo "Syncing source files"
rsyncexec $DICTIONNARY_PATH/fre/flex/mots-simples.txt leffe@calamondin:$REMOTE_DICTIONNARY_PATH/fre/flex/mots-simples.txt

echo "Remote installation"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/install-dictionnary.sh &

#local installation
# command configuration
echo "Compiling and installing dictionnary"
pushd $DICTIONNARY_PATH
./build.sh fre && ./deploy.sh fre
popd
