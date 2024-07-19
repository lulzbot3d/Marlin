/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2023 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../core/types.h"

typedef enum FXDTICtrlMode : uint8_t {
  ftMotionMode_DISABLED   =  0, // Standard Motion
  ftMotionMode_ENABLED    =  1  // Time-Based Motion
} ftMotionMode_t;

typedef enum FXDTICtrlCmpnstr : uint8_t {
  ftMotionCmpnstr_NONE       = 0, // No compensator
  ftMotionCmpnstr_ZV         = 1, // Zero Vibration
  ftMotionCmpnstr_ZVD        = 2, // Zero Vibration and Derivative
  ftMotionCmpnstr_ZVDD       = 3, // Zero Vibration, Derivative, and Double Derivative
  ftMotionCmpnstr_ZVDDD      = 4, // Zero Vibration, Derivative, Double Derivative, and Triple Derivative
  ftMotionCmpnstr_EI         = 5, // Extra-Intensive
  ftMotionCmpnstr_2HEI       = 6, // 2-Hump Extra-Intensive
  ftMotionCmpnstr_3HEI       = 7, // 3-Hump Extra-Intensive
  ftMotionCmpnstr_MZV        = 8  // Modified Zero Vibration
} ftMotionCmpnstr_t;

typedef enum FXDTICtrlTrajGenMode : uint8_t {
  trajGenMode_NONE       =  0U,
  trajGenMode_SWEEPC_X   =  1U,
  trajGenMode_SWEEPC_Y   =  2U,
  trajGenMode_ABORT      = 99U,
} ftMotionTrajGenMode_t;

typedef struct FXDTICtrlTrajGenConfig {
  ftMotionTrajGenMode_t mode = trajGenMode_NONE;
  float f0 = 0.0f,
        f1 = 0.0f,
        dfdt = 0.0f,
        a = 0.0f,
        pcws_ti[6] = {0.0f},
        k1 = 0.0f,
        k2 = 0.0f,
        step_ti = 0.0f,
        step_a = 0.0f,
        dly1_ti = 0.0f,
        dly2_ti = 0.0f,
        dly3_ti = 0.0f,
        step_a_x_0p5 = 0.0f,
        step_a_x_step_ti_x_step_ti = 0.0f,
        step_ti_x_2 = 0.0f,
        step_ti_x_3 = 0.0f,
        step_ti_x_4 = 0.0f;
} ftMotionTrajGenConfig_t;

typedef struct XYZEarray<float, FTM_WINDOW_SIZE> xyze_trajectory_t;
typedef struct XYZEarray<float, FTM_BATCH_SIZE> xyze_trajectoryMod_t;

enum {
  LIST_N(DOUBLE(LOGICAL_AXES),
    FT_BIT_DIR_E, FT_BIT_STEP_E,
    FT_BIT_DIR_X, FT_BIT_STEP_X, FT_BIT_DIR_Y, FT_BIT_STEP_Y, FT_BIT_DIR_Z, FT_BIT_STEP_Z,
    FT_BIT_DIR_I, FT_BIT_STEP_I, FT_BIT_DIR_J, FT_BIT_STEP_J, FT_BIT_DIR_K, FT_BIT_STEP_K,
    FT_BIT_DIR_U, FT_BIT_STEP_U, FT_BIT_DIR_V, FT_BIT_STEP_V, FT_BIT_DIR_W, FT_BIT_STEP_W
  ),
  FT_BIT_COUNT
};

typedef bits_t(FT_BIT_COUNT) ft_command_t;
