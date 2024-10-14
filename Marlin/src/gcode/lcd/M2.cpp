/**
 * Written By Brian Kahl 2023 - FAME3D
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

#include "../../inc/MarlinConfigPre.h"

#if HAS_END_PRINT_SCREEN

#include "../../inc/MarlinConfig.h"
#include "../gcode.h"

#if ENABLED(EXTENSIBLE_UI)
  #include "../../lcd/extui/ui_api.h"
  #include "../../lcd/extui/ftdi_eve_touch_ui/screens.h"
#endif

/**
 * M2: Go to end print screen
 *
 *  M2         - sending M2 brings up a screen that displays...
 *               "Would you like to start next print?"
 *               Yes and No buttons report back what decision was made.
 *
 *  M2 R       - Brings up a Reprint last job screen
 *
 *  M2 O       - Override the M2 end print page and bring the screen to Status Screen
 */
void GcodeSuite::M2() {

  if (parser.seen('O')){
    GOTO_SCREEN(StatusScreen);
  }
  else if (parser.seen('R')){
    #if ENABLED(EXTENSIBLE_UI)
      //if(ExtUI::isPrintingFromMedia())
      ExtUI::onReprintScreen(GET_TEXT_F(MSG_USERWAIT));
    #endif
  }
  else{
    #if ENABLED(EXTENSIBLE_UI)
      ExtUI::onPrintCompleteScreen(GET_TEXT_F(MSG_USERWAIT));
    #endif
  }
}

#endif // HAS_END_PRINT_SCREEN