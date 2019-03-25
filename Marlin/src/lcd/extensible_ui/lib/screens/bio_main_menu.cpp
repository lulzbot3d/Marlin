/*********************
 * bio_main_menu.cpp *
 *********************/

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

#if ENABLED(EXTENSIBLE_UI) && defined(LULZBOT_USE_BIOPRINTER_UI)

#include "screens.h"

using namespace FTDI;

void MainMenu::onRedraw(draw_mode_t what) {
  if(what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::background))
       .cmd(CLEAR(true,true,true));
  }

  if(what & FOREGROUND) {
    CommandProcessor cmd;
    default_button_colors();
    cmd.font(Theme::font_medium)
    #define GRID_ROWS 7
    #define GRID_COLS 2
      .tag(2).button( BTN_POS(1,1), BTN_SIZE(1,1), F("Auto Home"))
      .tag(3).button( BTN_POS(1,2), BTN_SIZE(1,1), F("Motors Off"))
      #if HAS_TRINAMIC
        .enabled(1)
      #else
        .enabled(0)
      #endif
      .tag(4).button( BTN_POS(1,3), BTN_SIZE(1,1), F("Motor mA"))
      #if HAS_TRINAMIC
        .enabled(1)
      #else
        .enabled(0)
      #endif
      .tag(5).button( BTN_POS(1,4), BTN_SIZE(1,1), F("Bump Sense"))
      .tag(6).button( BTN_POS(2,1), BTN_SIZE(1,1), F("Steps/mm"))
      .tag(7).button( BTN_POS(2,2), BTN_SIZE(1,1), F("Velocity "))
      .tag(8).button( BTN_POS(2,3), BTN_SIZE(1,1), F("Acceleration"))
    #if ENABLED(JUNCTION_DEVIATION)
      .tag(9).button( BTN_POS(2,4), BTN_SIZE(1,1), F("Junc Dev"))
    #else
      .tag(9).button( BTN_POS(2,4), BTN_SIZE(1,1), F("Jerk"))
    #endif
      //.tag(10).button( BTN_POS(1,5), BTN_SIZE(2,1), F("Interface Settings"))
      .tag(11).button( BTN_POS(1,5), BTN_SIZE(2,1), F("Restore Factory Defaults"))
      .tag(12).button( BTN_POS(1,6), BTN_SIZE(2,1), F("About Printer"))
      .style(LIGHT_BTN)
      .tag(1).button( BTN_POS(1,7), BTN_SIZE(2,1), F("Back"));
    #undef GRID_COLS
    #undef GRID_ROWS
  }
}

bool MainMenu::onTouchEnd(uint8_t tag) {
  using namespace ExtUI;

  switch(tag) {
    case 1:  GOTO_PREVIOUS();                                         break;
    case 2:  enqueueCommands_P(PSTR("G28"));                          break;
    case 3:  enqueueCommands_P(PSTR("M84"));                          break;
    case 4:  GOTO_SCREEN(StepperCurrentScreen);                       break;
    case 5:  GOTO_SCREEN(StepperBumpSensitivityScreen);               break;
    case 6:  GOTO_SCREEN(StepsScreen);                                break;
    case 7:  GOTO_SCREEN(MaxVelocityScreen);                          break;
    case 8:  GOTO_SCREEN(DefaultAccelerationScreen);                  break;
    case 9:
      #if ENABLED(JUNCTION_DEVIATION)
        GOTO_SCREEN(JunctionDeviationScreen);
      #else
        GOTO_SCREEN(JerkScreen);
      #endif
      break;
    //case 10: GOTO_SCREEN(InterfaceSettingsScreen);                    break;
    case 11: GOTO_SCREEN(RestoreFailsafeDialogBox);                   break;
    case 12: GOTO_SCREEN(AboutScreen);                                break;
    default:
      return false;
  }
  return true;
}

#endif // EXTENSIBLE_UI