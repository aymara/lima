##!bin/sh

# ssh and rsync configuration
source $PELF_COMMANDS_ROOT/sshcommands.sh

# command configuration
RESOURCE_PATH=$LINGUISTIC_DATA_ROOT
SYNRULES_PATH=$RESOURCE_PATH/syntacticAnalysis
REMOTE_SYNRULES_PATH=/home/leffe/Lima/Sources/linguisticData/syntacticAnalysis

echo "Syncing source files"
rsyncexec $SYNRULES_PATH/rules-l2r-fre-homodeps.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-l2r-fre-homodeps.txt
rsyncexec $SYNRULES_PATH/rules-r2l-fre-homodeps.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-r2l-fre-homodeps.txt
rsyncexec $SYNRULES_PATH/rules-fre-heterodeps.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-heterodeps.txt
rsyncexec $SYNRULES_PATH/rules-r2l-pass1-fre-homodeps.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-r2l-pass1-fre-homodeps.txt
rsyncexec $SYNRULES_PATH/simplification-first-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-first-rules-fre.txt
rsyncexec $SYNRULES_PATH/simplification-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-rules-fre.txt
rsyncexec $SYNRULES_PATH/simplification-last-rules-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/simplification-last-rules-fre.txt
rsyncexec $SYNRULES_PATH/rules-l2r-fre-pleonasticPronouns.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-l2r-fre-pleonasticPronouns.txt
rsyncexec $SYNRULES_PATH/categoriesClassesDeclaration-fre.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/categoriesClassesDeclaration-fre.txt
rsyncexec $SYNRULES_PATH/rules-fre-heterodeps-specializer.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-fre-heterodeps-specializer.txt
rsyncexec $SYNRULES_PATH/rules-l2r-fre-dummy.txt leffe@calamondin:$REMOTE_SYNRULES_PATH/rules-l2r-fre-dummy.txt

echo "Remote installation"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/install-synrules.sh
