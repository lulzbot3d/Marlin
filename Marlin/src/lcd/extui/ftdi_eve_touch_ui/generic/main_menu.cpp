/*****************
 * main_menu.cpp *
 *****************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2019 - Cocoa Press                          *
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

#ifdef FTDI_MAIN_MENU

using namespace FTDI;
using namespace Theme;

void MainMenu::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::bg_color))
       .cmd(CLEAR(true,true,true));
  }

  #if ENABLED(TOUCH_UI_PORTRAIT)
    #define GRID_COLS 2
    #define GRID_ROWS 8
    #define MOVE_AXIS_POS         BTN_POS(1,1), BTN_SIZE(1,1)
    #define DISABLE_STEPPERS_POS  BTN_POS(2,1), BTN_SIZE(1,1)
    #define TEMPERATURE_POS       BTN_POS(1,2), BTN_SIZE(1,1)
    #define CLEAN_NOZZLE_POS      BTN_POS(2,2), BTN_SIZE(1,1)
    #define LEVELING_POS          BTN_POS(1,3), BTN_SIZE(1,1)
    #define Z_OFFSET_POS          BTN_POS(2,3), BTN_SIZE(1,1)
    #define FLOWRATE_POS          BTN_POS(1,4), BTN_SIZE(1,1)
    #define RUNOUT_SENSOR_POS     BTN_POS(2,4), BTN_SIZE(1,1)
    #define ABOUT_PRINTER_POS     BTN_POS(1,5), BTN_SIZE(2,1)
    #define SETTINGS_POS          BTN_POS(1,6), BTN_SIZE(2,1)
    #define ADVANCED_SETTINGS_POS BTN_POS(1,7), BTN_SIZE(2,1)
    #define BACK_POS              BTN_POS(1,8), BTN_SIZE(2,1)
  #else
    #define GRID_COLS 6
    #define GRID_ROWS 5
    #define ADVANCED_SETTINGS_POS BTN_POS(1,1), BTN_SIZE(3,1)
    #define ABOUT_PRINTER_POS     BTN_POS(4,1), BTN_SIZE(3,1)
    #define MOVE_AXIS_POS         BTN_POS(1,3), BTN_SIZE(3,1)
    #define DISABLE_STEPPERS_POS  BTN_POS(4,3), BTN_SIZE(3,1)
    #define TEMPERATURE_POS       BTN_POS(1,4), BTN_SIZE(2,1)
    #define RUNOUT_SENSOR_POS     BTN_POS(1,4), BTN_SIZE(2,1)
    #define LEVELING_POS          BTN_POS(1,5), BTN_SIZE(3,1)
    #define BACK_POS              BTN_POS(4,5), BTN_SIZE(3,1)
  #endif

  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.colors(normal_btn)
       .font(Theme::font_medium)
       .tag(2).button(MOVE_AXIS_POS,         GET_TEXT_F(MSG_MOVE_AXIS))
       .tag(3).button(DISABLE_STEPPERS_POS,  F("Disable Motors"))
       .tag(5).button(CLEAN_NOZZLE_POS,      GET_TEXT_F(MSG_CLEAN_NOZZLE))
       .tag(4).button(FLOWRATE_POS,          GET_TEXT_F(MSG_FLOW))
       .tag(6).button(TEMPERATURE_POS,       GET_TEXT_F(MSG_TEMPERATURE))
       .tag(7).button(RUNOUT_SENSOR_POS,     GET_TEXT_F(MSG_RUNOUT_SENSOR))
       .tag(8).button(ADVANCED_SETTINGS_POS, GET_TEXT_F(MSG_ADVANCED_SETTINGS))
       .enabled(ENABLED(HAS_LEVELING))
       .tag(9).button(LEVELING_POS,          GET_TEXT_F(MSG_LEVELING))
       .tag(10).button(Z_OFFSET_POS,         GET_TEXT_F(MSG_ZOFFSET))
       .tag(11).button(ABOUT_PRINTER_POS,    GET_TEXT_F(MSG_INFO_MENU))
       .tag(12).button(SETTINGS_POS,         F("Printer Settings Overview"))
       .colors(action_btn)
       .tag(1).button(BACK_POS,              GET_TEXT_F(MSG_BUTTON_DONE));
  }
}

bool MainMenu::onTouchEnd(uint8_t tag) {
  using namespace ExtUI;

  switch (tag) {
    case 1:  SaveSettingsDialogBox::promptToSaveSettings();              break;
    case 2:  GOTO_SCREEN(MoveAxisScreen);                                break;
    case 3:  injectCommands(F("M84"));                                   break;
    case 4:  GOTO_SCREEN(FlowPercentScreen);                             break;
    case 5:
      GOTO_SCREEN(StatusScreen);
      #if ENABLED(MANUAL_NOZZLE_CLEAN)
        injectCommands(F("G12"));
      #else
         injectCommands(F(CLEAN_SCRIPT));
      #endif
      break;
    case 6:  GOTO_SCREEN(TemperatureScreen);                             break;
    case 7:  GOTO_SCREEN(FilamentRunoutScreen);                          break;
    case 8:  GOTO_SCREEN(AdvancedSettingsMenu);                          break;
    #if HAS_LEVELING
      case 9:  GOTO_SCREEN(LevelingMenu);                                break;
    #endif
    case 10:
      #if EXTRUDERS > 1
        GOTO_SCREEN(NudgeNozzleScreen);
      #else
        GOTO_SCREEN(ZOffsetScreen);
      #endif
      break;
    case 11: GOTO_SCREEN(AboutScreen);                                   break;
    case 12: GOTO_SCREEN(CalInfoScreen); break;
    default:
      return false;
  }
  return true;
}

#endif // FTDI_MAIN_MENU
