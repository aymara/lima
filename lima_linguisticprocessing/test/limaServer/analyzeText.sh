#!/bin/bash

set -o errexit
set -o pipefail

host=localhost
port=8080
pipeline=deepud

#!/bin/bash

while [ -n "$1" ]; do # while loop starts
    case "$1" in


    -h)
        host="$2"
        echo "-h option passed, with value $host"
        shift
        ;;

    -P)
        port="$2"
        echo "-P option passed, with value $port"
        shift
        ;;

    -p)
        pipeline="$2"
        echo "-p option passed, with value $pipeline"
        shift
        ;;

    --)
        shift # The double dash makes them parameters
        break
        ;;

    *)
        break;;
    esac

    shift
done

set -o nounset

if [ $# != 2 ]; then # If first parameter passed then print Hi

    echo "Must pass two parameters: language and text file to analyze."
    exit 1
fi
lang=$1
textfile=$2

curl -X POST -H "Content-Type application/x-www-form-urlencoded; charset=utf-8" -d "@${textfile}" "http://${host}:${port}/?lang=${lang}&pipeline=${pipeline}"
