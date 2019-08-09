#!/bin/sh

####
# Copyright (C) 2019  AlephObjects, Inc.
#
#
# The bash script in this page is free software: you can
# redistribute it and/or modify it under the terms of the GNU Affero
# General Public License (GNU AGPL) as published by the Free Software
# Foundation, either version 3 of the License, or (at your option)
# any later version.  The code is distributed WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU AGPL for more details.
#

####
# usage
#
# Prints out a usage summary
#
usage() {
  echo
  echo "Usage: $0 [-i|--increment] [-c|--commit]"
  echo
  echo Shows or manipulates the version of the firmware in the repo
  echo
  echo "   -i|--increment    Increments to the next version number"
  echo "   -c|--commit       Commits a change to the version number"
  echo
  exit
}

############################################
# MAIN SCRIPT
############################################

# Parse command line options

while true
do
  case $1 in
    -i|--increment)
      INCREMENT=1
      shift
      ;;
    -c|--commit)
      COMMIT=1
      shift
      ;;
    -*|--*)
      usage
      ;;
    *)
      break
      ;;
  esac
done

SRC=Marlin/src/inc/Version.h
fw_version=`grep  -m 1 "define SHORT_BUILD_VERSION" $SRC | cut -d \" -f 2`
fw_revision=`grep  -m 1 "define LULZBOT_FW_VERSION" $SRC | cut -d \" -f 2 | cut -d . -f 2`

if [ $INCREMENT ]; then
  fw_revision=`expr $fw_revision + 1`
fi

if [ $COMMIT ]; then
  sed -i "s/LULZBOT_FW_VERSION \".[0-9]\\+\"/LULZBOT_FW_VERSION \".$fw_revision\"/" $SRC
  git add $SRC
  git commit -m "Change version number" -m "Changed version number to $fw_version.$fw_revision"
fi

echo $fw_version.$fw_revision