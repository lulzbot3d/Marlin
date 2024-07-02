/****************************************************************************
 *   Written By Brian Kahl 2023 - FAME3D.                                   *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <http://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

#include "../../../inc/MarlinConfig.h"

#if ENABLED(SHOW_TOOL_HEAD_ID)

#include "../../gcode.h"
#include "../../../feature/tool_head_id.h"
#include "../../../module/planner.h"
/**
 * M891: Set the Tool Head ID value.
 *
 *   T<Tool Head ID>     Set value of Tool Head Id (1 to 20)
 *
 * Type M891 without any arguments to show active value.
 */
void GcodeSuite::M891() {
  bool noArgs = true;

  if (parser.seen('T')) {
    planner.synchronize();
    tool_head.setToolHeadId(parser.value_linear_units());
    noArgs = false;
  }

  if (noArgs) {
    #if ENABLED(TOOLHEAD_Galaxy_Series)
      SERIAL_ECHOPGM("0=No Tool Head selected 8=MET175 9=MET285 10=AST285");
    #elif ENABLED(TOOLHEAD_Legacy_Universal)
      SERIAL_ECHOPGM("0=No Tool Head selected 1=M175 2=SL 3=SE 4=HE 5=HS 6=HS+ 7=H175");
    #elif ANY(TOOLHEAD_Twin_Nebula_175, TOOLHEAD_Twin_Nebula_285)
      SERIAL_ECHOPGM("0=No Tool Head selected 11=Twin Nebula 175, 12=Twin Nebula 285");
    #elif ENABLED(TOOLHEAD_Quiver_DualExtruder)
      SERIAL_ECHOPGM("0=No Tool Head selected 13=Legacy Dual Extruder");
    #endif
    SERIAL_ECHOLNPGM("  Tool Head ID:", tool_head.getToolHeadId());
  }
}

void GcodeSuite::M891_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_TOOLHEAD_ID));
  SERIAL_ECHOLNPGM("  M891 T", tool_head.getToolHeadId());
}

#endif