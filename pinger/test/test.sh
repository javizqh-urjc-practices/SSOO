#!/bin/bash

RED="\033[01;31m"
GREEN="\033[01;32m"
BLANK="\033[01;00m"

STATUS=0

echo -e "----------STARTING TESTING------------\n"

echo -e "\t\tTEST 1\n"
pinger 3 ubuntu.com
if [ $(echo $?) -eq 0 ]; then
  echo -e "$GREEN\nCorrect: pinger 3 ubuntu.com$BLANK\n"
else 
  echo -e "$RED\nFailed: pinger 3 ubuntu.com$BLANK\n"
  STATUS=$((STATUS+1))
fi

echo -e "\t\tTEST 2\n"
pinger 3 ubuntu.com goggle.es
if [ $(echo $?) -eq 0 ]; then
  echo -e "$GREEN\nCorrect: pinger 3 ubuntu.com goggle.es$BLANK\n"
else 
  echo -e "$RED\nFailed: pinger 3 ubuntu.com goggle.es$BLANK\n"
  STATUS=$((STATUS+1))
fi

echo -e "\t\tTEST 3\n"
pinger 3 ubuntu.com goggle.notfound
if [ $(echo $?) -eq 1 ]; then
  echo -e "$GREEN\nCorrect: pinger 3 ubuntu.com goggle.notfound$BLANK\n"
else 
  echo -e "$RED\nFailed: pinger 3 ubuntu.com goggle.notfound$BLANK\n"
  STATUS=$((STATUS+1))
fi

echo -e "\t\tTEST 4\n"
pinger ubuntu.com goggle.notfound
if [ $(echo $?) -eq 1 ]; then
  echo -e "$GREEN\nCorrect: pinger ubuntu.com goggle.notfound$BLANK\n"
else 
  echo -e "$RED\nFailed: pinger ubuntu.com goggle.notfound$BLANK\n"
  STATUS=$((STATUS+1))
fi

echo -e "----------FINISHED TESTING------------\n"
if [ $STATUS -eq 0 ]; then
  echo -e "$GREEN" "Summary: all tests passed$BLANK"
else
  echo -e "$RED" "Summary: $STATUS tests failed$BLANK"
fi
exit 0