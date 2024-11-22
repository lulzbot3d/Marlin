/**
 * Written By Brian Kahl 2024 - FAME3D
 *
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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

#include "../../../inc/MarlinConfig.h"
#include "../../gcode.h"
#include "../../parser.h"
#include "../../../module/temperature.h"
#include "../../../module/planner.h"

#if ENABLED(CUSTOM_MATERIAL_PURGE_PATTERN)

void GcodeSuite::M8100()
{
  const int16_t old_fp = planner.flow_percentage[active_extruder];

  if (parser.seen('N')){
    feedrate_mm_s = (DEFAULT_PURGE_PATTERN_FEEDRATE / (max(parser.floatval('N'), 0.4) * 2));
  }
  else {
    feedrate_mm_s = DEFAULT_PURGE_PATTERN_FEEDRATE;
  }

  if (parser.seenval('F')){
    const float filament_diameter = parser.floatval('F');
    const float flow_value = (-56.636 * filament_diameter + 199.113);

    planner.set_flow(active_extruder, flow_value); //if the filament diameter is 2.85 set flow to 37.7% to extrude the same amount as 1.75
  }

  if (thermalManager.degTargetHotend(active_extruder) > EXTRUDE_MINTEMP) {
    if (parser.seen('M')){
      home_if_needed();
      const bool seenS = parser.seenval('M');
      if (seenS) {
        switch (parser.value_int()) {
          case 1:
            #if defined(MATERIAL_1_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_1_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 2:
            #if defined(MATERIAL_2_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_2_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 3:
            #if defined(MATERIAL_3_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_3_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 4:
            #if defined(MATERIAL_4_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_4_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 5:
            #if defined(MATERIAL_5_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_5_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 6:
            #if defined(MATERIAL_6_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_6_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 7:
            #if defined(MATERIAL_7_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_7_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 8:
            #if defined(MATERIAL_8_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_8_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;
          case 9:
            #if defined(MATERIAL_9_PURGE_PATTERN)
              process_subcommands_now(F(MATERIAL_9_PURGE_PATTERN));
            #else
              SERIAL_ERROR_MSG(STR_UNKNOWN_FILAMENT_PURGE_PATTERN);
            #endif
            break;

          default: break;
        }
      }
    }
  }
  else{
    SERIAL_ECHOLNPGM("Nozzle too Cold");
  }

  if (planner.flow_percentage[active_extruder] != old_fp)
    planner.set_flow(active_extruder, old_fp); // reset flow to original flow percentage
}
#endif // NOZZLE_CLEAN_FEATURE