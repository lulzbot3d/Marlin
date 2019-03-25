/*************************
 * bio_status_screen.cpp *
 *************************/

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

#include "../ftdi_eve_lib/extras/polygon.h"
#include "bio_printer_ui.h"

#define GRID_COLS 4
#define GRID_ROWS 9

using namespace FTDI;
using namespace Theme;
using namespace ExtUI;

void StatusScreen::onRedraw(draw_mode_t what) {
  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(background));
  cmd.cmd(CLEAR(true,true,true));
  cmd.tag(0);

  #define POLY(A) A, sizeof(A)/sizeof(A[0])

  Polygon p(cmd, 0, 0, display_width, display_height);

  // Paint the shadows for the syringe and buttons
  cmd.cmd(COLOR_RGB(shadow_rgb));
  cmd.cmd(VERTEX_TRANSLATE_X(5 * 16));
  cmd.cmd(VERTEX_TRANSLATE_Y(5 * 16));
  //p.fill(POLY(usb_btn));
  //p.fill(POLY(menu_btn));
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

  b.button(1, POLY(x_neg));
  b.button(2, POLY(x_pos));
  b.button(3, POLY(y_neg));
  b.button(4, POLY(y_pos));
  b.button(5, POLY(z_neg));
  b.button(6, POLY(z_pos));
  b.button(7, POLY(e_neg));
  b.button(8, POLY(e_pos));

  uint16_t x, y, h, v;
  cmd.cmd(COLOR_RGB(default_btn::rgb_enabled));
  cmd.fgcolor(fill_rgb).font(28);

  p.bounds(POLY(usb_btn), x, y, h, v);
  cmd.enabled(isMediaInserted() && !isPrintingFromMedia())
     .tag(9).button(x, y, h, v,
      isPrintingFromMedia() ?
        F("Printing") :
      #if ENABLED(USB_FLASH_DRIVE_SUPPORT)
        F("USB Drive")
      #else
        F("SD Card")
      #endif
  );

  p.bounds(POLY(menu_btn), x, y, h, v);
  cmd.tag(10).button(x, y, h, v, F("Menu"));
}

bool StatusScreen::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case  9: GOTO_SCREEN(FilesScreen); return true;
    case 10: GOTO_SCREEN(MainMenu);    return true;
    default:                           return false;
  }
}

bool StatusScreen::onTouchHeld(uint8_t tag) {
  if(ExtUI::isMoving()) return false; // Don't allow moves to accumulate
  constexpr float increment = 1;
  #define UI_INCREMENT_AXIS(axis) MoveAxisScreen::setManualFeedrate(axis, increment); UI_INCREMENT(AxisPosition_mm, axis);
  #define UI_DECREMENT_AXIS(axis) MoveAxisScreen::setManualFeedrate(axis, increment); UI_DECREMENT(AxisPosition_mm, axis);
  switch(tag) {
    case 1: UI_DECREMENT_AXIS(X); break;
    case 2: UI_INCREMENT_AXIS(X); break;
    case 3: UI_DECREMENT_AXIS(Y); break;
    case 4: UI_INCREMENT_AXIS(Y); break;
    case 5: UI_DECREMENT_AXIS(Z); break;
    case 6: UI_INCREMENT_AXIS(Z); break;
    case 7: UI_DECREMENT_AXIS(E0); break;
    case 8: UI_INCREMENT_AXIS(E0); break;
    default: return false;
  }
  #undef UI_DECREMENT_AXIS
  #undef UI_INCREMENT_AXIS
  return true;
}

#endif // EXTENSIBLE_UI