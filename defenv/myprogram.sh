#!/usr/bin/env bash

function usage(){
	echo -e 'Usage: myprogram.sh <params> \n' 1>&2
	exit 1
}

if [ $# = 0 ]; then
  usage
fi

for arg in $@; do
  echo -ne "$arg\t"
done

echo
printenv
exit 0