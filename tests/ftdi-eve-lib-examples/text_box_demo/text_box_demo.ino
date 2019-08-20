/*****************
 * text_demo.ino *
 *****************/

/****************************************************************************
 *   Written By Marcio Teixeira 2019 - Aleph Objects, Inc.                  *
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

#include "src/ftdi_eve_lib/ftdi_eve_lib.h"

using namespace FTDI;

/***************** SCREEN DEFINITIONS *****************/

class MainScreen : public UIScreen, public UncachedScreen {
  public:
    static void onRedraw(draw_mode_t what);
    static bool onTouchEnd(uint8_t tag);
};

void MainScreen::onRedraw(draw_mode_t what) {
  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(0x555555));
  cmd.cmd(CLEAR(true,true,true));

  #define GRID_COLS 4
  #define GRID_ROWS 7

  cmd.cmd(COLOR_RGB(0xFF9999)).rectangle(BTN_POS(1,1), BTN_SIZE(4,2))
     .cmd(COLOR_RGB(0x99FF99)).rectangle(BTN_POS(1,3), BTN_SIZE(4,2))
     .cmd(COLOR_RGB(0x9999FF)).rectangle(BTN_POS(1,5), BTN_SIZE(4,2));

  cmd.cmd(COLOR_RGB(0x000000));
  draw_text_box(cmd, BTN_POS(1,1), BTN_SIZE(4,2), F("This is a test\n\nwith a newline!"));
  draw_text_box(cmd, BTN_POS(1,3), BTN_SIZE(4,2), F("This is a test of the word wrapping function"), OPT_CENTER);
  draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(4,2), F("This is a test of the word wrapping function with more words"), OPT_CENTERY | OPT_RIGHTX);
  
  cmd.font(29).tag(1).button(BTN_POS(2,7), BTN_SIZE(2,1), F("Okay"));
}
    
bool MainScreen::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1:
      return true;
    default:
      return false;
  }
}

/***************** LIST OF SCREENS *****************/

SCREEN_TABLE {
  DECL_SCREEN(MainScreen)
};
SCREEN_TABLE_POST

/***************** MAIN PROGRAM *****************/

void setup() {
  Serial.begin(9600);

  EventLoop::setup();
  CLCD::turn_on_backlight();
}

void loop() {
  EventLoop::loop();
}
