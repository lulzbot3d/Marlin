/*********************
 * bioprinter_ui.ino *
 *********************/

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
#include "src/bioprinter_ui.h"
#include "src/polygon.h"

using namespace FTDI;

/***************** SCREEN DEFINITIONS *****************/

class LogoScreen : public UIScreen, public UncachedScreen {
  public:
    static void onRedraw(draw_mode_t what) {
      constexpr uint32_t background_rgb = 0xFFFFFF;
      constexpr uint32_t foreground_rgb = 0xC1D82F;

      CommandProcessor cmd;
      cmd.cmd(CLEAR_COLOR_RGB(background_rgb));
      cmd.cmd(CLEAR(true,true,true));
      cmd.tag(0);

      #define POLY(A) A, sizeof(A)/sizeof(A[0])

      Polygon p(cmd, x_min, y_min, x_max, y_max);

      constexpr uint32_t shadow_rgb  = 0xF3E0E0;
      constexpr uint32_t fill_rgb    = 0x00AA00;
      constexpr uint32_t stroke_rgb  = 0x005500;
      constexpr uint32_t syringe_rgb = 0xE2F2DA;

      // Paint the shadows for the syringe and buttons
      cmd.cmd(COLOR_RGB(shadow_rgb));
      cmd.cmd(VERTEX_TRANSLATE_X(5 * 16));
      cmd.cmd(VERTEX_TRANSLATE_Y(5 * 16));
      p.fill(POLY(usb_btn));
      p.fill(POLY(menu_btn));
      p.fill(POLY(syringe_outline));
      cmd.cmd(VERTEX_TRANSLATE_X(0));
      cmd.cmd(VERTEX_TRANSLATE_Y(0));

      // Paint the syring icon
      cmd.cmd(COLOR_RGB(syringe_rgb));
      p.fill(POLY(syringe_outline));
      cmd.cmd(COLOR_RGB(fill_rgb));
      p.fill(POLY(syringe_fluid));
      cmd.cmd(COLOR_RGB(stroke_rgb));
      p.fill(POLY(syringe));

      // Draw the arrow push buttons

      PolyButton b(p);
      b.fill_style(fill_rgb);
      b.stroke_style(stroke_rgb, 28);
      b.shadow_style(shadow_rgb, 5);
      
      b.button(1, POLY(x_pos));
      b.button(2, POLY(y_pos));
      b.button(3, POLY(z_pos));
      b.button(4, POLY(e_pos));
      b.button(5, POLY(x_neg));
      b.button(6, POLY(y_neg));
      b.button(7, POLY(z_neg));
      b.button(8, POLY(e_neg));

      uint16_t x, y, h, v;
      cmd.cmd(COLOR_RGB(stroke_rgb));
      cmd.fgcolor(fill_rgb);
      
      p.bounds(POLY(usb_btn), x, y, h, v);
      cmd.font(28).tag(9).button(x, y, h, v, F("USB Drive"));

      p.bounds(POLY(menu_btn), x, y, h, v);
      cmd.font(28).tag(10).button(x, y, h, v, F("Menu"));
    }

    static bool LogoScreen::onTouchEnd(uint8_t tag) {
      switch(tag) {
        case 1: return true;
        default: return false;
      }
    }
};

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
