/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../Marlin.h"

#if ENABLED(LULZBOT_BIOPRINTER_GCODE)

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/temperature.h"

#define E_MIN_PIN             X_MAX_PIN
#define E_MIN_PIN_INVERTING   LULZBOT_NORMALLY_CLOSED_ENDSTOP

/**
 * G112 runs the extruder backwards until an endstop is triggered
 */

static inline bool read_pin() {
  return (READ(E_MIN_PIN) != E_MIN_PIN_INVERTING);
}

static void home_e() {
  feedrate_mm_s = 1; // mm/s

  // Back off the extruder until the pin is triggered
  set_destination_from_current();
  while(!read_pin()) {
    destination[E_AXIS] -= 0.5;
    prepare_move_to_destination();
    planner.synchronize();
  };

  // Set E position to zero
  current_position[E_AXIS] = 0;
  sync_plan_position();
}

/**
 * G112: Bioprinter control
 *
 *   no args     - Home E
 */
void GcodeSuite::G112() {
  home_e();
}

#endif // LULZBOT_BIOPRINTER_GCODE
