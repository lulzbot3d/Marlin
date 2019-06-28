/****************************
 * bio_confirm_home_xyz.cpp *
 ****************************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
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

#include "../config.h"

#if ENABLED(EXTENSIBLE_UI)

#include "screens.h"

using namespace FTDI;

void BioConfirmHomeE::onRedraw(draw_mode_t what) {
  drawMessage(
    F("About to re-home plunger."),
    F("Remove syringe prior to"),
    F("proceeding. Continue?")
  );
  drawYesNoButtons();
}

bool BioConfirmHomeE::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1:
      SpinnerDialogBox::show(F("Please wait..."));
      SpinnerDialogBox::enqueueAndWait_P(PSTR(
        "G112\n"                            /* Home extruder */
        LULZBOT_MENU_AXIS_LEVELING_COMMANDS /* Level X axis */
        "G0 X115 Z50 F6000\n"               /* Goto loading position */
        "M400\n"                            /* Wait for moves to finish */
        "M18 X Y"                           /* Unlock motors */
      ));
      // Change the screen we will return to
      current_screen.forget();
      PUSH_SCREEN(StatusScreen);
      return true;
    case 2:
      GOTO_SCREEN(StatusScreen);
      return true;
    default:
      return DialogBoxBaseClass::onTouchEnd(tag);
  }
}
#endif // EXTENSIBLE_UI