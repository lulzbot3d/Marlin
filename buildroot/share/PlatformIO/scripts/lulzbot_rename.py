'''
 * Written By Carl Smith 2025 - FAME3D
 * 
 * This script is used to rename the firmware binary after building.
 * The new filename includes the build environment, firmware version, 
 * Git commit hash, and a flag for uncommitted changes.
 * Add to the platformio.ini build environment as a script.
 * extra_scripts = post:buildroot/share/PlatformIO/scripts/lulzbot_rename.py
 *
'''

import os
import re
import subprocess
import shutil  # For safe file operations

Import("env")

# Define project directories
PROJECT_DIR = env["PROJECT_DIR"]  # Root directory of the project
BUILD_DIR = os.path.join(env["PROJECT_BUILD_DIR"], env["PIOENV"])  # Correct build directory

# Path to firmware binary
firmware_path = os.path.join(BUILD_DIR, "firmware.bin")

# Get the build environment name
build_env = env["PIOENV"]

# Path to configuration.h (Located in Marlin folder)
config_file = os.path.join(PROJECT_DIR, "Marlin", "configuration.h")

# Extract LULZBOT_FW_VERSION from configuration.h
fw_version = "unknown"
if os.path.exists(config_file):
    with open(config_file, "r", encoding="utf-8") as f:
        for line in f:
            match = re.match(r'#define\s+LULZBOT_FW_VERSION\s+"(.+)"', line)
            if match:
                fw_version = match.group(1)
                break

# Get the Git commit hash (short version)
git_hash = "unknown"
try:
    git_hash = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).decode("utf-8").strip()
except Exception:
    git_hash = "unknown"

# Check for uncommitted changes
try:
    git_status = subprocess.check_output(["git", "status", "--porcelain"]).decode("utf-8").strip()
    has_uncommitted_changes = bool(git_status)  # True if there are uncommitted changes
except Exception:
    has_uncommitted_changes = True  # Assume changes if Git is unavailable

# Construct the firmware filename
new_firmware_name = f"Marlin_{build_env}_{fw_version}_{git_hash}"
if has_uncommitted_changes:
    new_firmware_name += "_modified"
new_firmware_name += ".bin"

# Define new firmware path
new_firmware_path = os.path.join(BUILD_DIR, new_firmware_name)

# Callback function to rename the firmware **after linking**
def rename_firmware(*args, **kwargs):
    if os.path.exists(firmware_path):
        if os.path.exists(new_firmware_path):
            os.remove(new_firmware_path)  # Remove existing file to prevent errors
        shutil.move(firmware_path, new_firmware_path)  # Move instead of rename
        print(f"Renamed firmware to {new_firmware_path}")
    else:
        print(f"Error: Firmware file not found at {firmware_path}")

# Hook into post-build action (AFTER linking)
env.AddPostAction(firmware_path, rename_firmware)
