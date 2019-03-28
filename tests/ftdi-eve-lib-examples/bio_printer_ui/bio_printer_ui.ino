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
#include "src/ftdi_eve_lib/extras/poly_ui.h"
#include "src/bio_printer_ui.h"

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

      #define POLY(A) PolyUI::poly_reader_t(A, sizeof(A)/sizeof(A[0]))

      PolyUI ui(cmd);

      constexpr uint32_t shadow_rgb  = 0xF3E0E0;
      constexpr uint32_t fill_rgb    = 0x00AA00;
      constexpr uint32_t stroke_rgb  = 0x005500;
      constexpr uint32_t syringe_rgb = 0xE2F2DA;

      // Paint the shadow for the syringe
      ui.color(shadow_rgb);
      ui.shadow(POLY(syringe_outline), 5);

      // Paint the syring icon
      ui.color(syringe_rgb);
      ui.fill(POLY(syringe_outline));
      ui.color(fill_rgb);
      ui.fill(POLY(syringe_fluid));
      ui.color(stroke_rgb);
      ui.fill(POLY(syringe));

      // Draw the arrow push buttons

      ui.button_fill  (fill_rgb);
      ui.button_stroke(stroke_rgb, 28);
      ui.button_shadow(shadow_rgb, 5);

      ui.button(1, POLY(x_neg));
      ui.button(2, POLY(x_pos));
      ui.button(3, POLY(y_neg));
      ui.button(4, POLY(y_pos));
      ui.button(5, POLY(z_neg));
      ui.button(6, POLY(z_pos));
      ui.button(7, POLY(e_neg));
      ui.button(8, POLY(e_pos));

      int16_t x, y, h, v;
      ui.color(stroke_rgb);
      cmd.fgcolor(fill_rgb);

      ui.bounds(POLY(usb_btn), x, y, h, v);
      cmd.font(28).tag(9).button(x, y, h, v, F("USB Drive"));

      ui.bounds(POLY(menu_btn), x, y, h, v);
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
