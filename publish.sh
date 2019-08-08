# A helper script file that updates the FW version number, tags and pushes it.

SCRIPT=config/examples/AlephObjects/build-config.py

# Get the current version number
THEIR_VERSION=`grep -m1 "#define SHORT_BUILD_VERSION " Marlin/src/inc/Version.h | cut -d \" -f 2`
LULZ_VERSION=`grep -m1 "FW_VERSION " $SCRIPT | cut -d \" -f 2 | cut -d . -f 2`

git tag v$THEIR_VERSION.$LULZ_VERSION HEAD
git push origin v$THEIR_VERSION.$LULZ_VERSION
git push

NEXT_VERSION=`expr $LULZ_VERSION + 1`
echo "Increment version to $NEXT_VERSION?"
select yn in "Yes" "No"; do
  case $yn in
    Yes)
      sed -i "s/FW_VERSION = \".[0-9]\\+\"/FW_VERSION = \".$NEXT_VERSION\"/" $SCRIPT
      git add $SCRIPT
      git commit -m "Change version number" -m "Changed version number to $THEIR_VERSION.$NEXT_VERSION"
      break;;
    No)
      NEXT_VERSION=$LULZ_VERSION
      break;;
  esac
done