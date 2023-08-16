/********************
 * about_screen.cpp *
 ********************/

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

#define GRID_COLS 4
#define GRID_ROWS 30

using namespace FTDI;
using namespace Theme;
using namespace ExtUI;

void AboutScreen::onEntry() {
  BaseScreen::onEntry();
  sound.play(chimes, PLAY_ASYNCHRONOUS);
}

void AboutScreen::onRedraw(draw_mode_t) {
  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);


  draw_text_box(cmd, BTN_POS(1,2), BTN_SIZE(4,5), F(
      #if defined(LULZBOT_LONG_BED)
      ""LULZBOT_LCD_MACHINE_NAME"\nWith Long bed"
      #elif defined(LULZBOT_BLTouch) && !defined(LULZBOT_LONG_BED)
      ""LULZBOT_LCD_MACHINE_NAME"\nWith BLTouch"
      #endif
  ), OPT_CENTER, font_xxlarge);

  //cmd.tag(2);
  draw_text_box(cmd, BTN_POS(1,7), BTN_SIZE(4,3), F(
        "Firmware:"
  ), OPT_CENTER, font_xlarge);

  draw_text_box(cmd, BTN_POS(1,10), BTN_SIZE(4,2), F(
         ""LULZBOT_LCD_TOOLHEAD_NAME""
  ), OPT_CENTER, font_xlarge);

  draw_text_box(cmd, BTN_POS(1,13), BTN_SIZE(4,3), F(
    "Place holder:"
  ), OPT_CENTER, font_xlarge);
  draw_text_box(cmd, BTN_POS(1,16), BTN_SIZE(4,2), F(
    ""LULZBOT_LCD_TOOLHEAD_NAME""
  ), OPT_CENTER, font_xlarge);

  draw_text_box(cmd, BTN_POS(1,19), BTN_SIZE(4,3), F(
        "Version:"
  ), OPT_CENTER, font_xlarge);

  draw_text_box(cmd, BTN_POS(1,22), BTN_SIZE(4,2), F(
    "Marlin "SHORT_BUILD_VERSION""
  ), OPT_CENTER, font_xlarge);

  draw_text_box(cmd, BTN_POS(1,24), BTN_SIZE(4,2), F(
      "www.lulzbot.com"
  ), OPT_CENTER, font_xlarge);

  cmd.font(font_medium).colors(action_btn).tag(1).button(BTN_POS(2,27), BTN_SIZE(2,3), F("Okay"));
}

bool AboutScreen::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1: GOTO_PREVIOUS();            return true;
#if ENABLED(DEVELOPER_SCREENS)
    case 2: GOTO_SCREEN(DeveloperMenu); return true;
#endif
    default:                            return false;
  }
}

#endif // EXTENSIBLE_UI