/*****************************
 * advance_settings_menu.cpp *
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

#ifdef FTDI_ADVANCED_SETTINGS_MENU

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

void AdvancedSettingsMenu::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::bg_color))
       .cmd(CLEAR(true,true,true));
  }

    #define GRID_COLS 2
    #if ENABLED(TOUCH_UI_PORTRAIT)
      #if NONE(HAS_MULTI_HOTEND, SENSORLESS_HOMING)
        #define GRID_ROWS 7
      #elif DISABLED(HAS_MULTI_HOTEND) && ENABLED(SENSORLESS_HOMING)
        #define GRID_ROWS 8
        #define TMC_HOMING_THRS_POS     BTN_POS(1,5), BTN_SIZE(2,1)
      #elif ENABLED(HAS_MULTI_HOTEND) && DISABLED(SENSORLESS_HOMING)
        #define GRID_ROWS 8
        #define OFFSETS_POS             BTN_POS(1,5), BTN_SIZE(2,1)
      #elif ENABLED(HAS_MULTI_HOTEND) && ENABLED(SENSORLESS_HOMING)
        #define GRID_ROWS 8
        #define OFFSETS_POS             BTN_POS(1,5), BTN_SIZE(1,1)
        #define TMC_HOMING_THRS_POS     BTN_POS(2,5), BTN_SIZE(1,1)
      #endif
      #define BACKLASH_POS            BTN_POS(1,1), BTN_SIZE(1,1)
      #define VELOCITY_POS            BTN_POS(2,1), BTN_SIZE(1,1)
      #define STEPS_PER_MM_POS        BTN_POS(1,2), BTN_SIZE(1,1)
      #define ACCELERATION_POS        BTN_POS(2,2), BTN_SIZE(1,1)
      #define FILAMENT_POS            BTN_POS(1,3), BTN_SIZE(1,1)
      #define JERK_POS                BTN_POS(2,3), BTN_SIZE(1,1)
      #define ENDSTOPS_POS            BTN_POS(1,4), BTN_SIZE(1,1)
      #define TMC_CURRENT_POS         BTN_POS(2,4), BTN_SIZE(1,1)
      #define RESTORE_DEFAULTS_POS    BTN_POS(1,GRID_ROWS-2), BTN_SIZE(2,1)
      #define INTERFACE_POS           BTN_POS(1,GRID_ROWS-1), BTN_SIZE(1,1)
      #define DISPLAY_POS             BTN_POS(2,GRID_ROWS-1), BTN_SIZE(1,1)
      #define BACK_POS                BTN_POS(1,GRID_ROWS),   BTN_SIZE(2,1)
    #else
      #define GRID_COLS 3
      #define GRID_ROWS 6
      #define CAL_INFO_POS            BTN_POS(1,1), BTN_SIZE(1,1)
      #define FLOW_POS                BTN_POS(1,4), BTN_SIZE(1,1)
      #define STEPS_PER_MM_POS        BTN_POS(2,1), BTN_SIZE(1,1)
      #define TMC_CURRENT_POS         BTN_POS(3,1), BTN_SIZE(1,1)
      #define TMC_HOMING_THRS_POS     BTN_POS(3,2), BTN_SIZE(1,1)
      #define BACKLASH_POS            BTN_POS(3,3), BTN_SIZE(1,1)
      #define FILAMENT_POS            BTN_POS(1,3), BTN_SIZE(1,1)
      #define ENDSTOPS_POS            BTN_POS(3,4), BTN_SIZE(1,1)
      #define DISPLAY_POS             BTN_POS(3,5), BTN_SIZE(1,1)
      #define INTERFACE_POS           BTN_POS(1,5), BTN_SIZE(2,1)
      #define RESTORE_DEFAULTS_POS    BTN_POS(1,6), BTN_SIZE(2,1)
      #define VELOCITY_POS            BTN_POS(2,2), BTN_SIZE(1,1)
      #define ACCELERATION_POS        BTN_POS(2,3), BTN_SIZE(1,1)
      #define JERK_POS                BTN_POS(2,4), BTN_SIZE(1,1)
      #define OFFSETS_POS             BTN_POS(1,2), BTN_SIZE(1,1)
      #define BACK_POS                BTN_POS(3,6), BTN_SIZE(1,1)
    #endif

  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.colors(normal_btn)
       .font(Theme::font_medium)
      .enabled(ENABLED(HAS_BED_PROBE))
      .tag(3) .button(STEPS_PER_MM_POS,       GET_TEXT_F(MSG_STEPS_PER_MM))
      .enabled(ENABLED(HAS_TRINAMIC_CONFIG))
      .tag(13).button(TMC_CURRENT_POS,        GET_TEXT_F(MSG_TMC_CURRENT))
      #if ENABLED(SENSORLESS_HOMING)
      .tag(14).button(TMC_HOMING_THRS_POS,    GET_TEXT_F(MSG_TMC_HOMING_THRS))
      #endif
      #if ENABLED(HAS_MULTI_HOTEND)
      .tag(4) .button(OFFSETS_POS,            GET_TEXT_F(MSG_OFFSETS_MENU))
      #endif
      .enabled(ANY(LIN_ADVANCE, FILAMENT_RUNOUT_SENSOR))
      .tag(11).button(FILAMENT_POS,           GET_TEXT_F(MSG_LINEAR_ADVANCE))
      .tag(12).button(ENDSTOPS_POS,           GET_TEXT_F(MSG_LCD_ENDSTOPS))
      .tag(15).button(DISPLAY_POS,            F("Touch Calibrate"))
      .tag(9) .button(INTERFACE_POS,          F("UI Settings"))
      .tag(10).button(RESTORE_DEFAULTS_POS,   GET_TEXT_F(MSG_RESTORE_DEFAULTS))
      .tag(5) .button(VELOCITY_POS,           F("Max Speed"))
      .tag(6) .button(ACCELERATION_POS,       GET_TEXT_F(MSG_ACCELERATION))
      .tag(7) .button(JERK_POS,               GET_TEXT_F(TERN(HAS_JUNCTION_DEVIATION, MSG_JUNCTION_DEVIATION, MSG_JERK)))
      .enabled(ENABLED(BACKLASH_GCODE))
      .tag(8).button(BACKLASH_POS,            GET_TEXT_F(MSG_BACKLASH))
      .colors(action_btn)
      .tag(1).button(BACK_POS,                GET_TEXT_F(MSG_BUTTON_DONE));
  }
}

bool AdvancedSettingsMenu::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case  1: SaveSettingsDialogBox::promptToSaveSettings(); break;
    case  2: GOTO_SCREEN(CalInfoScreen);              break;
    case  3: GOTO_SCREEN(StepsScreen);                break;
    #if HAS_MULTI_HOTEND
    case  4: GOTO_SCREEN(NozzleOffsetScreen);         break;
    #endif
    case  5: GOTO_SCREEN(MaxVelocityScreen);          break;
    case  6: GOTO_SCREEN(DefaultAccelerationScreen);  break;
    case  7: GOTO_SCREEN(TERN(HAS_JUNCTION_DEVIATION, JunctionDeviationScreen, JerkScreen)); break;
    #if ENABLED(BACKLASH_GCODE)
    case  8: GOTO_SCREEN(BacklashCompensationScreen); break;
    #endif
    case  9: GOTO_SCREEN(InterfaceSettingsScreen);  LockScreen::check_passcode(); break;
    case 10: GOTO_SCREEN(RestoreFailsafeDialogBox); LockScreen::check_passcode(); break;
    #if ANY(LIN_ADVANCE, FILAMENT_RUNOUT_SENSOR)
    case 11: GOTO_SCREEN(LinearAdvanceScreen); break;
    #endif
    case 12: GOTO_SCREEN(EndstopStatesScreen); break;
    #if HAS_TRINAMIC_CONFIG
    case 13: GOTO_SCREEN(StepperCurrentScreen); break;
    #endif
    #if ENABLED(SENSORLESS_HOMING)
    case 14: GOTO_SCREEN(StepperBumpSensitivityScreen); break;
    #endif
    case 15: GOTO_SCREEN(DisplayTuningScreen); break;
    case 16: GOTO_SCREEN(FlowPercentScreen);   break;
    case 17:
      GOTO_SCREEN(StatusScreen);
      #if ENABLED(MANUAL_NOZZLE_CLEAN)
        injectCommands(F("G12"));
      #else
         injectCommands(F(CLEAN_SCRIPT));
      #endif
      break;
    default: return false;
  }
  return true;
}

#endif // FTDI_ADVANCED_SETTINGS_MENU
