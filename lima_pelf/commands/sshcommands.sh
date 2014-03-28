##!bin/sh

# allow retry for ssh and rsync commands
TRIES_MAX=20
TRIES_SLEEP=5

# ssh command
SSH_CMD="ssh -i $HOME/.ssh/id_dsa"
sshexec(){
  (( TRIES=1 ))
  while (( TRIES <= $TRIES_MAX )); do
    echo "Shell command : $* (try $TRIES)"
    $SSH_CMD $* && break
    (( TRIES++ ))
    sleep $TRIES_SLEEP
  done
  if (( TRIES > $TRIES_MAX )); then exit 1; fi
}

# rsync command
rsyncexec(){
  (( TRIES=1 ))
  while (( TRIES <= $TRIES_MAX )); do
    echo "Syncing file : $* (try $TRIES)"
    rsync -czq --rsh="$SSH_CMD" $* && break
    (( TRIES++ ))
    sleep $TRIES_SLEEP
  done
  if (( TRIES > $TRIES_MAX )); then exit 1; fi
}
