/**
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

#if ENABLED(NOZZLE_CLEAN_FEATURE)

#include "../../../libs/nozzle.h"

#include "../../gcode.h"
#include "../../parser.h"
#include "../../../module/motion.h"
#if ENABLED(EXTENSIBLE_UI)
  #include "../../../lcd/extui/ui_api.h"
  using namespace ExtUI;
#endif

#if HAS_LEVELING
  #include "../../../module/planner.h"
  #include "../../../feature/bedlevel/bedlevel.h"
#endif

/**
 * G12: Clean the nozzle
 *
 *  E<bool>          : 0=Never or 1=Always apply the "software endstop" limits
 *  P0 S<strokes>    : Stroke cleaning with S strokes
 *  P1 Sn T<objects> : Zigzag cleaning with S repeats and T zigzags
 *  P2 Sn R<radius>  : Circle cleaning with S repeats and R radius
 *  X, Y, Z          : Specify axes to move during cleaning. Default: ALL.
 */
void GcodeSuite::G12() {
  #if ENABLED(MANUAL_NOZZLE_CLEAN)
    const uint8_t arrPos = ANY(SINGLENOZZLE, MIXING_EXTRUDER) ? 0 : active_extruder;
    xyz_pos_t park_point NOZZLE_CLEAN_POSITION;
    #if NOZZLE_CLEAN_MIN_TEMP > 20
      if (thermalManager.degTargetHotend(0) < NOZZLE_CLEAN_MIN_TEMP) {
        thermalManager.setTargetHotend(NOZZLE_CLEAN_MIN_TEMP, 0);
        SERIAL_ECHOLNPGM("Nozzle too Cold - Heating");
        ui.set_status(GET_TEXT_F(MSG_NOZZLE_TOO_COLD));
      }
      #if (HOTENDS == 2)
        if (thermalManager.degTargetHotend(1) < NOZZLE_CLEAN_MIN_TEMP) {
          thermalManager.setTargetHotend(NOZZLE_CLEAN_MIN_TEMP, 1);
          SERIAL_ECHOLNPGM("Nozzles too Cold - Heating");
          ui.set_status(GET_TEXT_F(MSG_NOZZLES_TOO_COLD));
        }
      #endif
      home_if_needed();
      thermalManager.wait_for_hotend(arrPos);
    #endif
    do_blocking_move_to(park_point);
    process_subcommands_now(F(MANUAL_NOZZLE_CLEAN_COMMANDS));
    KEEPALIVE_STATE(PAUSED_FOR_USER);
    wait_for_user = true;
    TERN_(EXTENSIBLE_UI, ExtUI::onUserConfirmRequired(GET_TEXT_F(MSG_CLEAN_NOZZLE)));
    while (wait_for_user) {idle_no_sleep();}
    process_subcommands_now(F(END_MANUAL_NOZZLE_CLEAN_COMMANDS));
  #else
    // Don't allow nozzle cleaning without homing first
    constexpr main_axes_bits_t clean_axis_mask = main_axes_mask & ~TERN0(NOZZLE_CLEAN_NO_Z, Z_AXIS) & ~TERN0(NOZZLE_CLEAN_NO_Y, Y_AXIS);
    if (homing_needed_error(clean_axis_mask)) return;

    #ifdef WIPE_SEQUENCE_COMMANDS
      if (!parser.seen_any()) {
        #if (HOTENDS == 1)
          process_subcommands_now(F(WIPE_SEQUENCE_COMMANDS));
        #else
          process_subcommands_now(F(WIPE_SEQUENCE_COMMANDS "\n" WIPE_SEQUENCE_2_COMMANDS));
        #endif
        return;
      }
    #endif

    const uint8_t pattern = (
      #if COUNT_ENABLED(NOZZLE_CLEAN_PATTERN_LINE, NOZZLE_CLEAN_PATTERN_ZIGZAG, NOZZLE_CLEAN_PATTERN_CIRCLE) > 1
        parser.ushortval('P', NOZZLE_CLEAN_DEFAULT_PATTERN)
      #else
        NOZZLE_CLEAN_DEFAULT_PATTERN
      #endif
    );
    const uint8_t strokes = parser.ushortval('S', NOZZLE_CLEAN_STROKES),
                  objects = TERN0(NOZZLE_CLEAN_PATTERN_ZIGZAG, parser.ushortval('T', NOZZLE_CLEAN_TRIANGLES));
    const float radius = TERN0(NOZZLE_CLEAN_PATTERN_CIRCLE, parser.linearval('R', NOZZLE_CLEAN_CIRCLE_RADIUS));

    const bool seenxyz = parser.seen("XYZ");
    const uint8_t cleans =  (!seenxyz || parser.boolval('X') ? _BV(X_AXIS) : 0)
                          | (!seenxyz || parser.boolval('Y') ? _BV(Y_AXIS) : 0)
                          | TERN(NOZZLE_CLEAN_NO_Z, 0, (!seenxyz || parser.boolval('Z') ? _BV(Z_AXIS) : 0))
                        ;

    #if HAS_LEVELING
      // Disable bed leveling if cleaning Z
      TEMPORARY_BED_LEVELING_STATE(!TEST(cleans, Z_AXIS) && planner.leveling_active);
    #endif

    SET_SOFT_ENDSTOP_LOOSE(!parser.boolval('E'));

    nozzle.clean(pattern, strokes, radius, objects, cleans);

    SET_SOFT_ENDSTOP_LOOSE(false);
  #endif
}

#endif // NOZZLE_CLEAN_FEATURE
