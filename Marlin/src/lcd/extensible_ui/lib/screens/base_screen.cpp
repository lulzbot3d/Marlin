/*******************
 * base_screen.cpp *
 *******************/

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

using namespace FTDI;
using namespace Theme;

void BaseScreen::onEntry() {
  CommandProcessor cmd;
  cmd.set_button_style_callback(buttonStyleCallback);
  UIScreen::onEntry();
}

bool BaseScreen::buttonStyleCallback(uint8_t tag, uint8_t &style, uint16_t &options, bool post) {
  if(post) {
    set_button_colors(normal_btn);
    return false;
  }

  #if defined(MENU_TIMEOUT)
    if(EventLoop::get_pressed_tag() != 0) {
      reset_menu_timeout();
    }
  #endif

  if(tag != 0 && EventLoop::get_pressed_tag() == tag) {
    options = OPT_FLAT;
  }

  CommandProcessor cmd;

  if(style & DISABLED_BTN) {
      cmd.tag(0);
      style &= ~DISABLED_BTN;
      set_button_colors(disabled_btn);
  } else {
    switch(style) {
      case ACTION_BTN:
        set_button_colors(action_btn);
        break;
      case RED_BTN:
        set_button_colors(red_btn);
        break;
      default:
        return false;
    }
  }
  return true; // Call me again to reset the colors
}

void BaseScreen::default_button_colors() {
  set_button_colors(normal_btn);
}

void BaseScreen::onIdle() {
  #if defined(MENU_TIMEOUT)
    const uint32_t elapsed = millis() - last_interaction;
    if(elapsed > uint32_t(MENU_TIMEOUT) * 1000) {
      GOTO_SCREEN(StatusScreen);
      reset_menu_timeout();
    }
  #endif
}

void BaseScreen::reset_menu_timeout() {
  #if defined(MENU_TIMEOUT)
    last_interaction = millis();
  #endif
}

#if defined(MENU_TIMEOUT)
  uint32_t BaseScreen::last_interaction;
#endif

#endif // EXTENSIBLE_UI