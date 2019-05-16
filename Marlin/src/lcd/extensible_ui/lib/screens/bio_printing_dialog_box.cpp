/*******************************
 * bio_printing_dialog_box.cpp *
 *******************************/

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

#include "../ftdi_eve_lib/extras/circular_progress.h"

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#define GRID_COLS 2
#define GRID_ROWS 9

void BioPrintingDialogBox::onRedraw(draw_mode_t what) {
  const uint32_t elapsed = getProgress_seconds_elapsed();
  const uint8_t hrs = elapsed/3600;
  const uint8_t min = (elapsed/60)%60;

  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .font(font_large)
     .text(BTN_POS(1,2), BTN_SIZE(2,1), F("Printing..."));

  cmd.tag(1)
     .font(font_xlarge);

  draw_circular_progress(cmd, BTN_POS(1,3), BTN_SIZE(2,4), getProgress_percent(), theme_dark, theme_darkest);

  char time_str[10];
  sprintf_P(time_str, PSTR("%02dh %02dm"), hrs, min);

  cmd.font(font_large)
     .text(BTN_POS(1,7), BTN_SIZE(2,1), time_str);

  cmd.colors(normal_btn)
     .font(font_medium)
      #if ENABLED(SDSUPPORT)
        .enabled(isPrintingFromMedia())
      #else
        .enabled(0)
      #endif
     .tag(2).button( BTN_POS(1,9), BTN_SIZE(1,1), F("Cancel"))
     .tag(isPrintingFromMediaPaused() ? 4 : 3)
     .colors(action_btn)
      #if ENABLED(SDSUPPORT)
        .enabled(isPrintingFromMedia())
      #else
        .enabled(0)
      #endif
        .button( BTN_POS(2,9), BTN_SIZE(1,1), isPrintingFromMediaPaused() ? F("Resume") : F("Pause"));
}

bool BioPrintingDialogBox::onTouchEnd(uint8_t tag) {
  switch(tag) {
    case 1: GOTO_SCREEN(FeedratePercentScreen);      break;
    case 2: GOTO_SCREEN(ConfirmAbortPrintDialogBox); break;
    case 3:  sound.play(twinkle, PLAY_ASYNCHRONOUS); ExtUI::pausePrint();  break;
    case 4:  sound.play(twinkle, PLAY_ASYNCHRONOUS); ExtUI::resumePrint(); break;
    default: return false;
  }
  return true;
}

void BioPrintingDialogBox::onIdle() {
  if(!isPrintingFromMedia())
    GOTO_SCREEN(StatusScreen);

  if(refresh_timer.elapsed(STATUS_UPDATE_INTERVAL)) {
    onRefresh();
    refresh_timer.start();
  }
  BaseScreen::onIdle();
}

void BioPrintingDialogBox::show() {
  GOTO_SCREEN(BioPrintingDialogBox);
}

#endif // EXTENSIBLE_UI