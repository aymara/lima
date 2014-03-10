##!bin/sh

# ssh and rsync configuration
source $LEFFE_COMMANDS_ROOT/sshcommands.sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
NGRAMS_PATH=$RESOURCE_PATH/disambiguisationMatrices/fre
REMOTE_NGRAMS_PATH=/home/leffe/Lima/Sources/linguisticData/disambiguisationMatrices/fre

echo "Syncing source files"
rsyncexec $NGRAMS_PATH/corpus/corpus_fr_merge.txt leffe@calamondin:$REMOTE_NGRAMS_PATH/corpus/corpus_fr_merge.txt

echo "Remote installation"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/install-ngrams.sh

echo "Retrieve modified ngrams"
rsyncexec leffe@calamondin:$REMOTE_NGRAMS_PATH/bigramsend_modif.txt $NGRAMS_PATH/bigramsend_modif.txt
rsyncexec leffe@calamondin:$REMOTE_NGRAMS_PATH/trigramsend_modif.txt $NGRAMS_PATH/trigramsend_modif.txt

#local installation
# command configuration
echo "Compiling and installing disambiguisation matrices (ngrams)"
pushd $NGRAMS_PATH
make clean && make distclean
cd ..
./build.sh fre && ./deploy.sh fre
popd
