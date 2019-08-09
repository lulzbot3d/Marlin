#!/bin/bash

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

# A helper script file to standardize the procedure of pushing
# changes to the repo: Tags, pushes and increments the version.

VERSION=`./version.sh`

# Tags and pushes the working directory to the repo

git tag v$VERSION
git push origin v$VERSION
git push

# Ask the user if they want to increment the version number
# (typically, the answer should be yes)

echo "Increment version to `./version.sh --increment`?"
select yn in "Yes" "No"; do
  case $yn in
    Yes)
      ./version.sh --increment --commit
      break;;
    No)
      break;;
  esac
done
