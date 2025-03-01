/*****************************
 * linear_advance_screen.cpp *
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
 *   location: <https://www.gnu.org/licenses/>.                             *
 ****************************************************************************/

#include "../config.h"
#include "../screens.h"

#ifdef FTDI_LINEAR_ADVANCE_SCREEN

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#define GRID_COLS 13
#define GRID_ROWS 10

void LinearAdvanceScreen::onRedraw(draw_mode_t what) {
  widgets_t w(what);
  CommandProcessor cmd;

  w.precision(2, DEFAULT_LOWEST).color(e_axis);
  w.heading(           GET_TEXT_F(MSG_LINEAR_ADVANCE));
  #if !HAS_MULTI_EXTRUDER
    w.adjuster(     2, GET_TEXT_F(MSG_LINEAR_ADVANCE_K),    getLinearAdvance_mm_mm_s(E0) );
  #else
    w.adjuster(     2, GET_TEXT_F(MSG_LINEAR_ADVANCE_K1), getLinearAdvance_mm_mm_s(E0) );
    w.adjuster(     4, GET_TEXT_F(MSG_LINEAR_ADVANCE_K2), getLinearAdvance_mm_mm_s(E1) );
    #if EXTRUDERS > 2
      w.adjuster(   6, GET_TEXT_F(MSG_LINEAR_ADVANCE_K3), getLinearAdvance_mm_mm_s(E2) );
      #if EXTRUDERS > 3
        w.adjuster( 8, GET_TEXT_F(MSG_LINEAR_ADVANCE_K4), getLinearAdvance_mm_mm_s(E3) );
      #endif
    #endif
  #endif
  w.increments();
  draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(13,5), F(
      "Linear Advance and K Value\n \n \n \n \n \n \n \n \n \n"
  ), OPT_CENTERY, font_large);
    draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(13,5), F(" \n \nLinear Advance tunes filament flow according to the print head's speed. A higher K value sharpens precision but can lead to over-extrusion; a lower value promotes smoother flow but may result in under-extrusion. This adjustment enhances print quality. \n \nFor tuning instructions, visit the Linear Advance section on the Marlin Firmware website."
  ), OPT_CENTERY, font_medium);
}

bool LinearAdvanceScreen::onTouchHeld(uint8_t tag) {
  using namespace ExtUI;
  const float increment = getIncrement();
  switch (tag) {
    case  2: UI_DECREMENT(LinearAdvance_mm_mm_s, E0); break;
    case  3: UI_INCREMENT(LinearAdvance_mm_mm_s, E0); break;
    #if HAS_MULTI_EXTRUDER
      case  4: UI_DECREMENT(LinearAdvance_mm_mm_s, E1);  break;
      case  5: UI_INCREMENT(LinearAdvance_mm_mm_s, E1); break;
      #if EXTRUDERS > 2
        case  6: UI_DECREMENT(LinearAdvance_mm_mm_s, E2);  break;
        case  7: UI_INCREMENT(LinearAdvance_mm_mm_s, E2);  break;
        #if EXTRUDERS > 3
          case  8: UI_DECREMENT(LinearAdvance_mm_mm_s, E3);  break;
          case  9: UI_INCREMENT(LinearAdvance_mm_mm_s, E3);  break;
        #endif
      #endif
    #endif
    default:
      return false;
  }
  SaveSettingsDialogBox::settingsChanged();
  return true;
}

#endif // FTDI_LINEAR_ADVANCE_SCREEN
