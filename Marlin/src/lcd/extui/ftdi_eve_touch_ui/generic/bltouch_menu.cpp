/*********************
 * bltouch_menu.cpp *
 *********************/

/****************************************************************************
 *   Written By Brian Kahl 2024 - FAME3D                                    *
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

#ifdef FTDI_BLTOUCH_MENU

#include "../../../../feature/bltouch.h"

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#if ENABLED(TOUCH_UI_PORTRAIT)
  #define GRID_COLS 2
  #define GRID_ROWS 8
  #define BLTOUCH_TITLE_POS  BTN_POS(1,1), BTN_SIZE(2,1)
  #define BLTOUCH_RESET_POS  BTN_POS(1,2), BTN_SIZE(1,1)
  #define BLTOUCH_TEST_POS   BTN_POS(2,2), BTN_SIZE(1,1)
  #define BLTOUCH_DEPLOY_POS BTN_POS(1,3), BTN_SIZE(1,1)
  #define BLTOUCH_STOW_POS   BTN_POS(2,3), BTN_SIZE(1,1)
  #define BACK_POS           BTN_POS(1,8), BTN_SIZE(2,1)
#else
  #define GRID_COLS 3
  #define GRID_ROWS 6
  #define BLTOUCH_TITLE_POS  BTN_POS(3,3), BTN_SIZE(1,1)
  #define BLTOUCH_RESET_POS  BTN_POS(3,2), BTN_SIZE(1,1)
  #define BLTOUCH_RESET_POS  BTN_POS(3,4), BTN_SIZE(1,1)
  #define BLTOUCH_TEST_POS   BTN_POS(3,5), BTN_SIZE(1,1)
  #define BLTOUCH_DEPLOY_POS BTN_POS(3,3), BTN_SIZE(1,1)
  #define BLTOUCH_STOW_POS   BTN_POS(3,3), BTN_SIZE(1,1)
  #define BACK_POS           BTN_POS(1,6), BTN_SIZE(3,1)
#endif

void BltouchMenu::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::bg_color))
       .cmd(CLEAR(true,true,true))
       .tag(0);
  }

  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.font(font_large)
       .cmd(COLOR_RGB(bg_text_enabled))
       .text(BLTOUCH_TITLE_POS, GET_TEXT_F(MSG_BLTOUCH))
       .font(font_medium).colors(normal_btn)
       .tag(2).button(BLTOUCH_RESET_POS, GET_TEXT_F(MSG_BLTOUCH_RESET))
       .tag(3).button(BLTOUCH_TEST_POS,  GET_TEXT_F(MSG_BLTOUCH_SELFTEST))
       .tag(4).button(BLTOUCH_DEPLOY_POS, GET_TEXT_F(MSG_BLTOUCH_DEPLOY))
       .tag(5).button(BLTOUCH_STOW_POS,  GET_TEXT_F(MSG_BLTOUCH_STOW))
       .colors(action_btn)
       .tag(1).button(BACK_POS, GET_TEXT_F(MSG_BUTTON_DONE));
  }
}

bool BltouchMenu::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1: GOTO_PREVIOUS(); break;
    case 2: injectCommands(F("M280 P0 S60")); break;
    case 3: SpinnerDialogBox::enqueueAndWait(F("M280 P0 S90\nG4 P100\nM280 P0 S120")); break;
    case 4: injectCommands(F("M401\nM140 S0")); break;
    case 5: injectCommands(F("M402")); break;
    default: return false;
  }
  return true;
}

#endif // FTDI_BLTOUCH_MENU
