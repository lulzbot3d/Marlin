/******************************
 * filament_runout_screen.cpp *
 ******************************/

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

#ifdef FTDI_FILAMENT_RUNOUT_SCREEN

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#define GRID_COLS 13
#define GRID_ROWS (8+EXTRUDERS)

void FilamentRunoutScreen::onRedraw(draw_mode_t what) {
  widgets_t w(what);
  CommandProcessor cmd;

  w.heading(   GET_TEXT_F(MSG_FILAMENT));
  w.toggle( 2, GET_TEXT_F(MSG_RUNOUT_SENSOR), getFilamentRunoutEnabled());

  #if HAS_FILAMENT_RUNOUT_DISTANCE
    w.heading(GET_TEXT_F(MSG_RUNOUT_DISTANCE_MM));
    w.units(GET_TEXT_F(MSG_UNITS_MM));
    w.precision(0);
    w.color(e_axis);
    w.adjuster( 10, FPSTR(NUL_STR), getFilamentRunoutDistance_mm(), getFilamentRunoutEnabled());
    w.increments();
    draw_text_box(cmd, BTN_POS(1,6), BTN_SIZE(13,3), F(
        "Filament Jam & Runout Sensor\n \n \n \n \n \n "
    ), OPT_CENTERY, font_large);
      draw_text_box(cmd, BTN_POS(1,6), BTN_SIZE(13,3), F(" \n \nAn encoder monitors filament, automatically pausing prints if it detects jams or runouts after a set Runout Distance in millimeters. This prevents waste and errors. \n \nFilament guide tube MUST be attached at both ends for proper operation.\n "
    ), OPT_CENTERY, font_medium);
  #endif
}

bool FilamentRunoutScreen::onTouchHeld(uint8_t tag) {
  using namespace ExtUI;
  #if HAS_FILAMENT_RUNOUT_DISTANCE
    const float increment = getIncrement();
  #endif
  switch (tag) {
    case 2: setFilamentRunoutEnabled(!getFilamentRunoutEnabled()); break;
    #if HAS_FILAMENT_RUNOUT_DISTANCE
      case  10: UI_DECREMENT(FilamentRunoutDistance_mm); break;
      case  11: UI_INCREMENT(FilamentRunoutDistance_mm); break;
    #endif
    default:
      return false;
  }

  SaveSettingsDialogBox::settingsChanged();
  return true;
}

#endif // FTDI_FILAMENT_RUNOUT_SCREEN
