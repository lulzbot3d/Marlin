/*****************
 * logo_demo.ino *
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
#include "src/ftdi_eve_lib/extras/poly_ui.h"
#include "src/LulzBot_Logo.h"

using namespace FTDI;

/***************** SCREEN DEFINITIONS *****************/

class LogoScreen : public UIScreen, public UncachedScreen {
  private:
    static uint32_t color;

  public:

    static void onRedraw(draw_mode_t what) {
      constexpr uint32_t background_rgb = 0xDEEA5C;
      constexpr uint32_t foreground_rgb = 0xC1D82F;

      CommandProcessor cmd;
      cmd.cmd(CLEAR_COLOR_RGB(background_rgb));
      cmd.cmd(CLEAR(true,true,true));

      #define POLY(A) PolyUI::poly_reader_t(A, sizeof(A)/sizeof(A[0]))

      PolyUI ui(cmd);
      cmd.cmd(COLOR_RGB(0xC1D82F));
      ui.fill(POLY(logo_green));
      cmd.cmd(COLOR_RGB(0x000000));
      ui.fill(POLY(logo_black));
      cmd.cmd(COLOR_RGB(color));
      ui.fill(POLY(logo_white));
      
      cmd.cmd(COLOR_RGB(0x000000));
      ui.fill(POLY(logo_type));
      cmd.cmd(COLOR_RGB(0x000000));
      ui.fill(POLY(logo_mark));

      #define GRID_COLS 4
      #define GRID_ROWS 9
      cmd.fgcolor(foreground_rgb);
      cmd.font(30).tag(1).button(BTN_POS(2,8), BTN_SIZE(2,1), F("Okay"));
    }

    static bool onTouchEnd(uint8_t tag) {
      switch(tag) {
        case 1: {
          color = random(0xFFFFFF);
          return true;
        }
        default: return false;
      }
    }
};

uint32_t LogoScreen::color = 0xFFFFFF;

/***************** LIST OF SCREENS *****************/

SCREEN_TABLE {
  DECL_SCREEN(LogoScreen),
};
SCREEN_TABLE_POST

/***************** MAIN PROGRAM *****************/

void setup() {
  Serial.begin(9600);
  EventLoop::setup();
  CLCD::turn_on_backlight();
  SoundPlayer::set_volume(255);
}

void loop() {
  EventLoop::loop();
}
