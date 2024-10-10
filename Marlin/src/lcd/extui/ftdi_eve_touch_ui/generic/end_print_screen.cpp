/**************************************
 * confirm_abort_print_dialog_box.cpp *
 **************************************/

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
 *   location: <https://www.gnu.org/licenses/>.                             *
 ****************************************************************************/

#include "../config.h"
#include "../screens.h"

#ifdef FTDI_END_PRINT_SCREEN

#include "../../../../feature/host_actions.h"

using namespace ExtUI;

void EndPrintScreenDialogBox::onRedraw(draw_mode_t) {
  drawStartPrintButtons();
}

bool EndPrintScreenDialogBox::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1:
      GOTO_PREVIOUS();
      injectCommands(F("M117 Q60"));
      return true;
    case 2:
      GOTO_PREVIOUS();
      injectCommands(F("M117 Q60 S"));
      return true;
    default:
      return DialogBoxBaseClass::onTouchEnd(tag);
  }
}

void EndPrintScreenDialogBox::show(const char *msg) {
  drawMessage(msg);
  if (!AT_SCREEN(EndPrintScreenDialogBox))
    GOTO_SCREEN(EndPrintScreenDialogBox);
}

void EndPrintScreenDialogBox::hide() {
  if (AT_SCREEN(EndPrintScreenDialogBox))
    GOTO_PREVIOUS();
}

#endif // FTDI_END_PRINT_SCREEN
