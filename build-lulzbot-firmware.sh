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
  echo "Usage: $0 [-s|--short-names] [-h|--hash] [-c|--config] [printer toolhead|config_file]"
  echo
  echo "   -s|--short-names  Omits LulzBot code names from generated firmware files"
  echo
  echo "   -h|--hash         Records md5sum of the firmware files. These files will be generated:"
  echo "                       md5sums-full*    Sum w/  embedded version str and timestamp"
  echo "                       md5sums-bare*    Sum w/o embedded version str and timestamp"
  echo
  echo "   -c|--config       Save the values of 'Configuration.h' and 'Configuration_adv.h'"
  echo "                     that are used for the specified printer and toolhead."
  echo "   -n|--dry-run      Just print commands, don't execute"
  echo
  exit
}

####
# compile_deps_for_board <board>
#
# Compiles dependencies for the specific board
#
compile_deps_for_board() {
  board=$1 ; shift 1

  # Only compile dependency once

  case "$deps_built" in
    *$board*) return ;;
  esac

  deps_built="$deps_built $board"

  # Compile dependency

  echo
  echo Building dependencies for $board
  echo

  case $board in
    BOARD_ARCHIM2)
      SRC="ArduinoAddons/arduino-1.8.5/packages/ultimachine/hardware/sam/1.6.9-b"
      (cd "$SRC/system/libsam/build_gcc"; ARM_GCC_TOOLCHAIN="$TOOLS_PATH" make)
      cp -u $SRC/variants/arduino_due_x/libsam_sam3x8e_gcc_rel.a     $SRC/variants/archim/libsam_sam3x8e_gcc_rel.a
      cp -u $SRC/variants/arduino_due_x/libsam_sam3x8e_gcc_rel.a.txt $SRC/variants/archim/libsam_sam3x8e_gcc_rel.a.txt
      ;;
  esac
}

####
# locate_gcc_for_board <board>
#
# Locates compilation tools for the specific board
#
locate_gcc_for_board() {
  board=$1 ; shift 1

  case $board in
    BOARD_ARCHIM2)
      locate_tools TOOLS_PATH arm-none-eabi
      check_tools $TOOLS_PATH arm-none-eabi
      ;;
    *)
      locate_tools TOOLS_PATH avr
      check_tools $TOOLS_PATH avr
      ;;
  esac
}

####
# get_config_info <config_dir>
#
# Generates a filename for the hex file in accordance to Lulzbot standards.
# Extracts version and motherboard information from the configuration files.
#
get_config_info() {
  config=$1; shift 1

  parent=`dirname $config`
  printer=`basename $parent`
  toolhead=`basename $config`
  fw_hash=`git rev-parse --verify HEAD --short`
  fw_version=`./version.sh`
  fw_filename=Marlin_${printer}_${toolhead}_${fw_version}_${fw_hash}
  motherboard_name=`grep "define MOTHERBOARD" $config/Configuration.h | awk '{print $3}'`
  motherboard_number=`grep "$motherboard_name\b" Marlin/src/core/boards.h | awk '{print $3}'`
  is_lulzbot=`grep "define LULZBOT_" $config/Configuration.h`
}

####
# compile_firmware [makeopts]
#
# Compiles firmware for the specified printer and toolhead
#
compile_firmware() {
  (cd Marlin; make clean; make \
    $MAKE_FLAGS \
    AVR_TOOLS_PATH=$TOOLS_PATH/ \
    ARDUINO_INSTALL_DIR=../ArduinoAddons/arduino-1.8.5 \
    ARDUINO_VERSION=10805 \
    HARDWARE_MOTHERBOARD=$motherboard_number \
    LULZBOT_EXTRAS="$EXTRA_OPTS" \
    DEFINES="$EXTRA_DEFS" \
    $*) || exit
}

####
# record_checksum <checksum-file-prefix>
#
# Records the md5sum of the compiled firmware to the checksum file
#
record_checksum() {
  cat Marlin/applet/Marlin.hex Marlin/applet/Marlin.bin | md5sum  | sed "s/-/${printer}_${toolhead}/g" >> ${1}-${fw_version}-${fw_hash}.txt
}

####
# build_firmware <config_dir>
#
# Compiles firmware for the specified printer and toolhead
#
build_firmware() {
  config=$1; shift 1

  if [ ! -f $config/Configuration.h ]; then
    echo Cannot locate $config
    return
  fi

  get_config_info $config
  locate_gcc_for_board $motherboard_name
  compile_deps_for_board $motherboard_name

  if [ -z "$is_lulzbot" ]; then
    # Bail if the FW is not an official Lulzbot build
    echo Skipping $config because it does not appear compatible with this script.
    return
  fi

  # Copy over the configuration files
  cp $config/Configuration.h $config/Configuration_adv.h Marlin

  # Generate hashes
  if [ $MAKE_HASHES ]; then
    echo
    echo Generating bare checksum for $printer and $toolhead
    echo
    EXTRA_OPTS='-Wfatal-errors -Wno-builtin-macro-redefined'
    # Hide timestamps and versions so binaries can be diffed
    EXTRA_DEFS='__DATE__=\"?\" __TIME__=\"?\" LULZBOT_FW_VERSION=\"?\"'
    compile_firmware
    record_checksum build/md5sums-bare
  fi

  # Compile the firmware
  echo
  echo Building for $printer and $toolhead
  echo
  EXTRA_OPTS='-Wfatal-errors'
  EXTRA_DEFS=''
  compile_firmware
  if [ $MAKE_HASHES ]; then
    record_checksum build/md5sums-full
  fi

  if [ $DRY_RUN ]; then
    return
  fi

  # Copy builds to build directory

  if [ $motherboard_name = "BOARD_ARCHIM2" ]; then
    mv Marlin/applet/Marlin.bin build/$fw_filename.bin
  else
    mv Marlin/applet/Marlin.hex build/$fw_filename.hex
  fi
  chmod a-x build/*

  if [ $GENERATE_CONFIG ]; then
    cp $config/Configuration_summary.txt build/$fw_filename.config
  fi
}

####
# check_tool <path> <exec_name>
#
# Checks whether a tool exists in path
#
check_tool() {
  if [ ! -x "$1/$2" ]; then
    echo Cannot locate $2 in $1.
    exit 1
  fi
}

####
# locate_tools <path_var> <prefix>
#
# Attempts to locate a tool, otherwise prompts
# the user for a location. The found path is
# stored in <path_var>
#
locate_tools() {
  DEST_VAR=$1
  TOOL_BINARY=`which $2-objcopy`
  if [ $? -eq 0 ]; then
    TOOLS_PATH=`dirname $TOOL_BINARY`
  fi
  while [ ! -x $TOOLS_PATH/$2-objcopy ]
  do
    echo
    echo $2-objcopy not found!
    echo
    read -p "Type path to $2 tools: " TOOLS_PATH
    if [ -z $TOOLS_PATH ]; then
      echo Aborting.
      exit
    fi
  done
  eval "$DEST_VAR=$TOOLS_PATH"
}

####
# check_tools <path> <prefix>
#
# Verify that all the AVR tools we need exist in the located
# directory.
#
check_tools() {
  echo
  echo Using $1 for $2 tools.
  echo

  check_tool $1 $2-gcc
  check_tool $1 $2-objcopy
  check_tool $1 $2-g++
  check_tool $1 $2-objdump
  check_tool $1 $2-ar
  check_tool $1 $2-size
}

####
# build_summary
#
# Print out a summary of hex files that were created
#
build_summary() {
  echo
  echo
  echo
  echo Generated files stored in "`pwd`/build":
  echo
  ls build
  echo
}

############################################
# MAIN SCRIPT
############################################

# Parse command line options

while true
do
  case $1 in
    -h|--hash)
      MAKE_HASHES=1
      shift
      ;;
    -s|--short-names)
      SHORTNAMES=1
      shift
      ;;
    -c|--config)
      GENERATE_CONFIG=1
      shift
      ;;
    -n|--dry-run)
      MAKE_FLAGS=-n
      DRY_RUN=1
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

MAKE_FLAGS="$MAKE_FLAGS -j $(grep -c ^processor /proc/cpuinfo)"

rm -rf build
mkdir  build

# Collect list of configuration directories to build

case $# in
  2)
    # If the user specified a printer and toolhead, try finding the config files
    CONFIG_DIRS="config/examples/AlephObjects/$1/$2"
    if [ ! -f "$CONFIG_DIRS/Configuration.h" ]; then
      CONFIG_DIRS="config/examples/AlephObjects/EXPERIMENTAL/$1/$2"
    fi
    ;;
  1)
    # If the user specified a configuration file, use that.
    CONFIG_DIRS=`dirname $1`
    ;;
  0)
    # Otherwise, build all AlephObjects configuration files
    CONFIG_DIRS=`find config/examples/AlephObjects/ -name Configuration.h -exec dirname {} \;`
    ;;
esac

# Build the firmware files

for i in $CONFIG_DIRS; do
  build_firmware $i
done

if [ $# -eq 0 ]; then
  # If compiling everything, clean up the config files after compilation is done
  git checkout Marlin/Configuration.h Marlin/Configuration_adv.h
fi

if [ $SHORTNAMES ]; then
  rename 's/Marlin_(.+)_(.+)_(.+)_(.+)_(.+)_(.+)/Marlin_$2_$4_$5_$6/'       build/*
fi

build_summary