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

#if ENABLED(TOOL_HEAD_ID)

#include "../../gcode.h"
#include "../../../feature/tool_head_id.h"
#include "../../../module/planner.h"
/**
 * M891: Set the Tool Head ID value.
 *
 *   T<Tool Head ID>     Set value of Tool Head Id (1 to 10)
 *
 * Type M891 without any arguments to show active value.
 */
void GcodeSuite::M891() {
  bool noArgs = true;
  
  if (parser.seen('T')) {
    planner.synchronize();
    toolhead.id = parser.value_linear_units();
    noArgs = false;
    }

  if (noArgs) {
    #if ENABLED(TOOLHEAD_Galaxy_Series)
      SERIAL_ECHOPGM("1=MET175 2=MET285 3=AST285");
    #elif ENABLED(TOOLHEAD_Legacy_Universal)
      SERIAL_ECHOPGM("1=SL 2=SE 3=HE 4=HS 5=HS+ 6=M175 7=H175");
    #endif
    SERIAL_ECHOLNPAIR("  Tool Head ID:", toolhead.id);
  }
}

#endif