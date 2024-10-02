/***************************************
 * stepper_bump_sensitivity_screen.cpp *
 **************************************/

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

#ifdef FTDI_STEPPER_BUMP_SENSITIVITY_SCREEN

#define GRID_COLS 13
#define GRID_ROWS (8+EXTRUDERS)

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

void StepperBumpSensitivityScreen::onRedraw(draw_mode_t what) {
  widgets_t w(what);
  CommandProcessor cmd;
  w.precision(0, BaseNumericAdjustmentScreen::DEFAULT_LOWEST);
  w.heading(                     GET_TEXT_F(MSG_TMC_HOMING_THRS));
  w.color(x_axis)  .adjuster( 2, GET_TEXT_F(MSG_AXIS_X),  getTMCBumpSensitivity(X), ENABLED(X_SENSORLESS));
  w.color(y_axis)  .adjuster( 4, GET_TEXT_F(MSG_AXIS_Y),  getTMCBumpSensitivity(Y), ENABLED(Y_SENSORLESS));
  w.color(z_axis)  .adjuster( 6, GET_TEXT_F(MSG_AXIS_Z),  getTMCBumpSensitivity(Z), ENABLED(Z_SENSORLESS));
  w.increments();

  draw_text_box(cmd, BTN_POS(1,6), BTN_SIZE(13,3), F(
        "Sensitivity:\n(+) Decrease / (-) Increase\n \n \n \n "
  ), OPT_CENTER, font_large);
    draw_text_box(cmd, BTN_POS(1,7), BTN_SIZE(13,3), F(
        "Detects 3D printer limits by sensing motor resistance;\nno mechanical endstops or sensors needed."
  ), OPT_CENTER, font_medium);
}

bool StepperBumpSensitivityScreen::onTouchHeld(uint8_t tag) {
  const float increment = getIncrement();
  switch (tag) {
    case  2: UI_DECREMENT(TMCBumpSensitivity, X); break;
    case  3: UI_INCREMENT(TMCBumpSensitivity, X); break;
    case  4: UI_DECREMENT(TMCBumpSensitivity, Y); break;
    case  5: UI_INCREMENT(TMCBumpSensitivity, Y); break;
    case  6: UI_DECREMENT(TMCBumpSensitivity, Z); break;
    case  7: UI_INCREMENT(TMCBumpSensitivity, Z); break;
    default:
      return false;
  }
  SaveSettingsDialogBox::settingsChanged();
  return true;
}

#endif // FTDI_STEPPER_BUMP_SENSITIVITY_SCREEN
