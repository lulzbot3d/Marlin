'''
 * Written By Carl Smith 2026 - FAME3D - with great help from ChatGPT
 * 
 * This script is used to create a snapshot of the Marlin configuration after building the firmware.
 * Add to the platformio.ini build environment as a script.
 * extra_scripts = post:buildroot/share/PlatformIO/scripts/lulzbot_config_snapshot.py
 *
 * This is modeled after an old "config" makefile target from the Aleph firmware years ago.
 * as seen here: https://github.com/lulzbot3d/Marlin/blob/v2.0.0.144/Marlin/Makefile#L783
'''

import os
import subprocess
import tempfile
from datetime import datetime

Import("env")

# --- Helpers -------------------------------------------------------------

def get_git_hash():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            stderr=subprocess.DEVNULL
        ).decode().strip()
    except:
        return "nogit"

def get_fw_version():
    # Try to extract from configuration.h
    config_path = os.path.join(env["PROJECT_DIR"], "Marlin", "Configuration.h")

    if not os.path.exists(config_path):
        return "path_unknown"

    with open(config_path, "r") as f:
        for line in f:
            if "LULZBOT_FW_VERSION" in line:
                parts = line.strip().split()
                if len(parts) >= 3:
                    return parts[2].strip('"')
    return "ver_unknown"

def format_defines_list(defines):
    result = []
    for d in defines:
        if isinstance(d, tuple):
            key, val = d

            if isinstance(val, str):
                val = val.replace('\\"', '"')
                val = env.subst(val)

            result.append(f"-D{key}={val}")
        else:
            result.append(f"-D{d}")
    return result

def preprocess_config(header):
    with tempfile.TemporaryDirectory(prefix="marlin_cfg_") as tmpdir:

        temp_cpp = os.path.join(tmpdir, "macros.cpp")
        temp_out = os.path.join(tmpdir, "preprocessed.h")

        with open(temp_cpp, "w", encoding="utf-8") as f:
            marlin_config = os.path.join(env["PROJECT_DIR"], "Marlin", "src", "inc", "MarlinConfig.h")
            f.write(f'#include "{marlin_config}"\n')
            with open(header, "r", encoding="utf-8", errors="ignore") as h:
                f.write(h.read())

        with open(temp_cpp, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()

        content = content.replace("#ifndef CONFIGURATION_H", "#ifndef FOO")
        content = content.replace("#ifndef CONFIGURATION_ADV_H", "#ifndef FOO")

        import re
        content = re.sub(r'^\s*#pragma once.*$', '', content, flags=re.MULTILINE)
        content = re.sub(r"#define\s+(\w+)", r"pound_defined_\1", content)

        with open(temp_cpp, "w", encoding="utf-8") as f:
            f.write(content)

        # Run preprocessor
        cmd = [
            env.subst("$CXX"),
            *env.subst("$CCFLAGS").split(),
            *env.subst("$CXXFLAGS").split(),
            *format_defines_list(env.get("CPPDEFINES", [])),
            *[f'-I{env.subst(p)}' for p in env.get("CPPPATH", [])],
            "-E",
            temp_cpp,
            "-o",
            temp_out
        ]

        result = subprocess.run(
            cmd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True
        )

        if result.returncode != 0:
            raise RuntimeError("Preprocessing failed")

        # Extract defines
        defines = []
        with open(temp_out, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                if "pound_defined_" in line:
                    line = line.strip()
                    line = re.sub(r"\s*pound_defined_(\w+)", r"#define \1", line)
                    defines.append(line)

        return defines

def get_git_modified():
    try:
        status = subprocess.check_output(
            ["git", "status", "--porcelain"],
            stderr=subprocess.DEVNULL
        ).decode().strip()
        return "_modified" if status else ""
    except:
        return "_git_unknown"

# --- Main hook -----------------------------------------------------------

def dump_config(source, target, env):
    env_name = env.subst("$PIOENV")

    git_hash = get_git_hash()
    fw_version = get_fw_version()
    git_modified = get_git_modified()

    filename = f"Marlin_{env_name}_{fw_version}_{git_hash}{git_modified}.config"

    # This firmware directory should be the same as where lulzbot_rename.py copies the firmware builds to.
    firmware_backup_dir = os.path.join(env["PROJECT_DIR"], "firmware_builds")
    os.makedirs(firmware_backup_dir, exist_ok=True)
    output_path = os.path.join(firmware_backup_dir, filename)

    config_h = env.subst("$PROJECT_DIR/Marlin/Configuration.h")
    config_adv = env.subst("$PROJECT_DIR/Marlin/Configuration_adv.h")

    all_lines = []

    all_lines.append("/* Configuration.h */")
    all_lines += preprocess_config(config_h)

    all_lines.append("\n/* Configuration_adv.h */")
    all_lines += preprocess_config(config_adv)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(all_lines))

    print(f"Config snapshot written to: {output_path}")

# Run after build
env.AddPostAction("buildprog", dump_config)
