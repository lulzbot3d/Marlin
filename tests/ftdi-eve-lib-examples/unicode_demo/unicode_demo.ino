/********************
 * unicode_demo.ino *
 ********************/

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

class FontScreen : public UIScreen, public UncachedScreen {
  public:
    static void onRedraw(draw_mode_t what) {
      #define GRID_COLS 4
      #define GRID_ROWS 7

      CommandProcessor cmd;
      cmd.cmd(CLEAR_COLOR_RGB(0xFFFFFF));
      cmd.cmd(CLEAR(true,true,true));
      cmd.tag(0);

      cmd.cmd(COLOR_RGB(0x000000));

      cmd.font(31)
         .text(BTN_POS(1,1), BTN_SIZE(4,1), F(u8"Héllò Wörld"))
         .font(29)
         .text(BTN_POS(1,2), BTN_SIZE(4,1), F(u8"ßåçaÝeøiñð 80°F X²"))
         .font(26)
         .text(BTN_POS(1,5), BTN_SIZE(4,1), F(u8"ÆÐØÞæðøþ¢£¤¥ºª±×÷¼½¾µ¶¦§¬¹²³"))
         .font(30)
         .tag(1)
         .button(BTN_POS(2,6), BTN_SIZE(2,1), F(u8"Ökæ"));

      cmd.cmd(COLOR_RGB(0xFF9999))
         .rectangle(BTN_POS(1,3), BTN_SIZE(4,2))
         .cmd(COLOR_RGB(0x000000));
      draw_text_box(cmd, BTN_POS(1,3), BTN_SIZE(4,2), F(u8"« La guerre, on le sait, fut pendant longtemps le plus sûr et le plus rapide véhicule de la civilisation. »\n\n- Jules Verne"), OPT_CENTERY);
    }

    static bool onTouchEnd(uint8_t tag) {
      switch(tag) {
        case 1: return true;
        default: return false;
      }
    }
};

/***************** LIST OF SCREENS *****************/

SCREEN_TABLE {
  DECL_SCREEN(FontScreen),
};
SCREEN_TABLE_POST

/***************** MAIN PROGRAM *****************/

void setup() {
  Serial.begin(9600);

  EventLoop::setup();

  load_utf8_data(0);

  CLCD::turn_on_backlight();
  SoundPlayer::set_volume(255);
}

void loop() {
  EventLoop::loop();
}
