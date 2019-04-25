/*****************************
 * dialog_box_base_class.cpp *
 *****************************/

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
using namespace Theme;

#define GRID_COLS 2
#define GRID_ROWS 8

template<typename T>
void DialogBoxBaseClass::drawMessage(const T line1, const T line2, const T line3, int16_t font) {
  T lines[] = {line1, line2, line3};
  const uint8_t n_lines = line3 ? 3 : line2 ? 2 : 1;
  CommandProcessor cmd;
  cmd.cmd(CMD_DLSTART)
     .cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);
  cmd.font(font ? font : font_large);
  for(uint8_t line = 0; line < n_lines; line++) {
    cmd.text( BTN_POS(1,3-n_lines/2+line), BTN_SIZE(2,1), lines[line]);
  }
  cmd.colors(normal_btn);
}

template void DialogBoxBaseClass::drawMessage(const char *, const char *, const char *, int16_t);
template void DialogBoxBaseClass::drawMessage(const progmem_str, const progmem_str, const progmem_str, int16_t);

void DialogBoxBaseClass::drawYesNoButtons() {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button( BTN_POS(1,8), BTN_SIZE(1,1), F("Yes"))
     .tag(2).button( BTN_POS(2,8), BTN_SIZE(1,1), F("No"));
}

void DialogBoxBaseClass::drawOkayButton() {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button( BTN_POS(1,8), BTN_SIZE(2,1), F("Okay"));
}

void DialogBoxBaseClass::drawButton(const progmem_str label) {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button( BTN_POS(1,8), BTN_SIZE(2,1), label);
}

void DialogBoxBaseClass::drawSpinner() {
  CommandProcessor cmd;
  cmd.cmd(COLOR_RGB(bg_text_enabled))
     .spinner(BTN_POS(1,4), BTN_SIZE(2,3)).execute();
}

bool DialogBoxBaseClass::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1: GOTO_PREVIOUS(); return true;
    case 2: GOTO_PREVIOUS(); return true;
    default:                 return false;
  }
}

#endif // EXTENSIBLE_UI