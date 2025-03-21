/***********************
 * cal_info_screen.cpp *
 ***********************/

/****************************************************************************
 *   Written By Carl Smith 2025 - FAME3D                                    *
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
#include "../../ui_api.h"

#ifdef FTDI_CAL_INFO_SCREEN

#define GRID_COLS 1

#if EXTRUDERS > 1
  #define GRID_ROWS 24
#else
  #define GRID_ROWS 22
#endif

using namespace FTDI;
using namespace Theme;
using namespace ExtUI;

void CalInfoScreen::onEntry() {
  BaseScreen::onEntry();
}

void CalInfoScreen::onRedraw(draw_mode_t) {
  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);

  // Not sure why I have to make all the text boxes two rows high to get the font size to work.
  draw_text_box(cmd, BTN_POS(1,1),  BTN_SIZE(1,2), F("Active Settings"), OPT_CENTERX, font_xlarge);

  char buffer[48];

  snprintf(buffer, sizeof(buffer), "Z Offset: %.2f", getZOffset_mm());
  draw_text_box(cmd, BTN_POS(1,3),  BTN_SIZE(1,2), buffer, 0, font_large);

  snprintf(buffer, sizeof(buffer), "Speed: %.0f%%", getFeedrate_percent());
  draw_text_box(cmd, BTN_POS(1,4), BTN_SIZE(1,2), buffer, 0, font_large);

  snprintf(buffer, sizeof(buffer), "Flowrate: %d%%", getFlow_percent(E0));
  draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(1,2), buffer, 0, font_large);

  snprintf(buffer, sizeof(buffer), "Filament Sensor:  OFF");  // Assume off, will overwrite OFF if sensor is on.
  if (getFilamentRunoutEnabled()) snprintf(buffer + 18, sizeof(buffer) - 18, "ON: %.0f mm", getFilamentRunoutDistance_mm());
  draw_text_box(cmd, BTN_POS(1,6), BTN_SIZE(1,2), buffer, 0, font_large);

  draw_text_box(cmd, BTN_POS(1,8),  BTN_SIZE(1,2), F("Backlash:"), 0, font_large);
  snprintf(buffer, sizeof(buffer), "   X: %.2f mm  Y: %.2f mm  Z: %.2f mm", getAxisBacklash_mm(X), getAxisBacklash_mm(Y), getAxisBacklash_mm(Z));
  draw_text_box(cmd, BTN_POS(1,9),  BTN_SIZE(1,2), buffer, 0, font_small);
  snprintf(buffer, sizeof(buffer), "   Smoothing: %.2f mm", getBacklashSmoothing_mm());
  draw_text_box(cmd, BTN_POS(1,10),  BTN_SIZE(1,2), buffer, 0, font_small);
  snprintf(buffer, sizeof(buffer), "   Correction: %.0f%%", getBacklashCorrection_percent());
  draw_text_box(cmd, BTN_POS(1,11),  BTN_SIZE(1,2), buffer, 0, font_small);

  draw_text_box(cmd, BTN_POS(1,13), BTN_SIZE(1,2), F("Linear Advance:"), 0, font_large);
  #if EXTRUDERS > 1
    snprintf(buffer, sizeof(buffer), "    K E1: %.2f  K E2: %.2f", getLinearAdvance_mm_mm_s(E0), getLinearAdvance_mm_mm_s(E1));
  #else
    snprintf(buffer, sizeof(buffer), "    K E1: %.2f", getLinearAdvance_mm_mm_s(E0));
  #endif
  draw_text_box(cmd, BTN_POS(1,14), BTN_SIZE(1,2), buffer, 0, font_small);

  draw_text_box(cmd, BTN_POS(1,16),  BTN_SIZE(1,2), F("Steps/mm:"), 0, font_large);
  snprintf(buffer, sizeof(buffer), "   X: %.2f  Y: %.2f  Z: %.2f", getAxisSteps_per_mm(X), getAxisSteps_per_mm(Y), getAxisSteps_per_mm(Z));
  draw_text_box(cmd, BTN_POS(1,17),  BTN_SIZE(1,2), buffer, 0, font_small);
  snprintf(buffer, sizeof(buffer), "   E: %.2f", getAxisSteps_per_mm(E0));
  draw_text_box(cmd, BTN_POS(1,18),  BTN_SIZE(1,2), buffer, 0, font_small);

  #if EXTRUDERS > 1
    draw_text_box(cmd, BTN_POS(1,20),  BTN_SIZE(1,2), F("Tool Offset:"), 0, font_large);
    snprintf(buffer, sizeof(buffer), "   X: %.2f mm  Y: %.2f mm  Z: %.2f mm", getNozzleOffset_mm(X, E1), getNozzleOffset_mm(Y, E1), getNozzleOffset_mm(Z, E1));
    draw_text_box(cmd, BTN_POS(1,21),  BTN_SIZE(1,2), buffer, 0, font_small);
  #endif

  #undef GRID_ROWS
  #define GRID_ROWS 9
  cmd.font(font_medium).colors(action_btn).tag(1).button(BTN_POS(1,9), BTN_SIZE(1,1), GET_TEXT_F(MSG_BUTTON_DONE));
 }

bool CalInfoScreen::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1: GOTO_PREVIOUS();  return true;
    default:                  return false;
   }
  return false;
}

#endif // EXTENSIBLE_UI