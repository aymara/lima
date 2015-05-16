##!bin/sh

# ssh and rsync configuration
source $PELF_COMMANDS_ROOT/sshcommands.sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
SYNRULES_PATH=$RESOURCE_PATH/syntacticAnalysis
REMOTE_SYNRULES_PATH=/home/leffe/Lima/Sources/linguisticData/syntacticAnalysis

echo "Syncing source files"
rsyncexec $SYNRULES_PATH/rules-fre-homodeps-pass1.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-homodeps-pass1.txt
rsyncexec $SYNRULES_PATH/rules-fre-homodeps-pass2.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-homodeps-pass2.txt
rsyncexec $SYNRULES_PATH/rules-fre-heterodeps.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-heterodeps.txt
rsyncexec $SYNRULES_PATH/rules-fre-homodeps-pass0.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-homodeps-pass0.txt
rsyncexec $SYNRULES_PATH/simplification-first-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-first-rules-fre.txt
rsyncexec $SYNRULES_PATH/simplification-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-rules-fre.txt
rsyncexec $SYNRULES_PATH/simplification-last-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-last-rules-fre.txt
rsyncexec $SYNRULES_PATH/rules-fre-pleonasticPronouns.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-pleonasticPronouns.txt
rsyncexec $SYNRULES_PATH/categoriesClassesDeclaration-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/categoriesClassesDeclaration-fre.txt
rsyncexec $SYNRULES_PATH/rules-fre-heterodeps-specializer.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-heterodeps-specializer.txt
rsyncexec $SYNRULES_PATH/rules-l2r-fre-dummy.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-l2r-fre-dummy.txt

echo "Remote installation"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/install-synrules.sh

#local installation
# command configuration
echo "Compiling and installing syntactic rules"
pushd $SYNRULES_PATH
make clean && make fre && make install-fre
popd
