/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../config.h"
#include "../screens.h"

#ifdef FTDI_CUSTOM_USER_MENUS

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#define _ITEM_TAG(N) (10+N)
#define _USER_DESC(N) MAIN_MENU_ITEM_##N##_DESC
#define _USER_GCODE(N) MAIN_MENU_ITEM_##N##_GCODE
#define _USER_ITEM(N) .tag(_ITEM_TAG(N)).button(USER_ITEM_POS(N), _USER_DESC(N))
#define _USER_ACTION(N) case _ITEM_TAG(N): injectCommands_P(PSTR(_USER_GCODE(N))); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen)); break;

void CustomUserMenus::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::bg_color))
       .cmd(CLEAR(true, true, true));
  }

   //#if HAS_USER_ITEM(16, 17, 18, 19, 20)
   //  #define _MORE_THAN_FIFTEEN 1
   //#else
    #define _MORE_THAN_FIFTEEN 0
   //#endif
   //#if _MORE_THAN_FIFTEEN || HAS_USER_ITEM(11, 12, 13, 14, 15)
   //  #define _MORE_THAN_TEN 1
   //#else
    #define _MORE_THAN_TEN 0
   //#endif

  #if ENABLED(TOUCH_UI_PORTRAIT)
    #define GRID_ROWS 11
    #define GRID_COLS (1 + _MORE_THAN_TEN)
    #define USER_ITEM_POS(N) BTN_POS((1+((N-1)/10)), ((N-1) % 10 + 1)), BTN_SIZE(1,1)
    #define BACK_POS         BTN_POS(1,11), BTN_SIZE(1,1)
  #else
    // #if _MORE_THAN_TEN || HAS_USER_ITEM(6, 7, 8, 9, 10)
       #define _MORE_THAN_FIVE 1
    // #else
    //  #define _MORE_THAN_FIVE 0
    //#endif
    #define GRID_ROWS 6
    #define GRID_COLS (1 + _MORE_THAN_FIVE + _MORE_THAN_TEN + _MORE_THAN_FIFTEEN)
    #define USER_ITEM_POS(N) BTN_POS((1+((N-1)/5)), ((N-1) % 5 + 1)), BTN_SIZE(1,1)
    #define BACK_POS         BTN_POS(1,6), BTN_SIZE(GRID_COLS,1)
  #endif

  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.colors(normal_btn)
       .font(Theme::font_medium)
       #if defined(MAIN_MENU_ITEM_3_DESC)
        //_USER_ITEM(1)
        .tag(_ITEM_TAG(11)).button(USER_ITEM_POS(1), MAIN_MENU_ITEM_3_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_4_DESC)
        //_USER_ITEM(2)
        .tag(_ITEM_TAG(12)).button(USER_ITEM_POS(2), MAIN_MENU_ITEM_4_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_5_DESC)
        //_USER_ITEM(3)
        .tag(_ITEM_TAG(13)).button(USER_ITEM_POS(3), MAIN_MENU_ITEM_5_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_6_DESC)
        //_USER_ITEM(4)
        .tag(_ITEM_TAG(14)).button(USER_ITEM_POS(4), MAIN_MENU_ITEM_6_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_7_DESC)
        //_USER_ITEM(5)
        .tag(_ITEM_TAG(15)).button(USER_ITEM_POS(5), MAIN_MENU_ITEM_7_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_8_DESC)
        //_USER_ITEM(6)
        .tag(_ITEM_TAG(16)).button(USER_ITEM_POS(6), MAIN_MENU_ITEM_8_DESC) 
       #endif
       #if defined(MAIN_MENU_ITEM_9_DESC)
        //_USER_ITEM(7)
        .tag(_ITEM_TAG(17)).button(USER_ITEM_POS(7), MAIN_MENU_ITEM_9_DESC) 
       #endif
       #if HAS_USER_ITEM(8)
        _USER_ITEM(8)
       #endif
       #if HAS_USER_ITEM(9)
        _USER_ITEM(9)
       #endif
       #if HAS_USER_ITEM(10)
        _USER_ITEM(10)
       #endif
       #if HAS_USER_ITEM(11)
        _USER_ITEM(11)
       #endif
       #if HAS_USER_ITEM(12)
        _USER_ITEM(12)
       #endif
       #if HAS_USER_ITEM(13)
        _USER_ITEM(13)
       #endif
       #if HAS_USER_ITEM(14)
        _USER_ITEM(14)
       #endif
       #if HAS_USER_ITEM(15)
        _USER_ITEM(15)
       #endif
       #if HAS_USER_ITEM(16)
        _USER_ITEM(16)
       #endif
       #if HAS_USER_ITEM(17)
        _USER_ITEM(17)
       #endif
       #if HAS_USER_ITEM(18)
        _USER_ITEM(18)
       #endif
       #if HAS_USER_ITEM(19)
        _USER_ITEM(19)
       #endif
       #if HAS_USER_ITEM(20)
        _USER_ITEM(20)
       #endif
      .colors(action_btn)
      .tag(1).button(BACK_POS, GET_TEXT_F(MSG_BACK));
  }
}

bool CustomUserMenus::onTouchEnd(uint8_t tag) {
  switch (tag) {
    #if defined(MAIN_MENU_ITEM_3_DESC)
      //_USER_ACTION(1)
      case _ITEM_TAG(11): injectCommands_P(PSTR(MAIN_MENU_ITEM_3_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_4_DESC)
      //_USER_ACTION(2)
      case _ITEM_TAG(12): injectCommands_P(PSTR(MAIN_MENU_ITEM_4_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_5_DESC)
      //_USER_ACTION(3)
      case _ITEM_TAG(13): injectCommands_P(PSTR(MAIN_MENU_ITEM_5_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_6_DESC)
      //_USER_ACTION(4)
      case _ITEM_TAG(14): injectCommands_P(PSTR(MAIN_MENU_ITEM_6_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_7_DESC)
      //_USER_ACTION(5)
      case _ITEM_TAG(15): injectCommands_P(PSTR(MAIN_MENU_ITEM_7_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_8_DESC)
      //_USER_ACTION(6)
      case _ITEM_TAG(16): injectCommands_P(PSTR(MAIN_MENU_ITEM_8_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if defined(MAIN_MENU_ITEM_9_DESC)
      //_USER_ACTION(7)
      case _ITEM_TAG(17): injectCommands_P(PSTR(MAIN_MENU_ITEM_9_GCODE)); TERN_(CUSTOM_MENU_MAIN_SCRIPT_RETURN, GOTO_SCREEN(StatusScreen));sound.play(chimes, PLAY_ASYNCHRONOUS); break;
    #endif
    #if HAS_USER_ITEM(8)
      _USER_ACTION(8)
    #endif
    #if HAS_USER_ITEM(9)
      _USER_ACTION(9)
    #endif
    #if HAS_USER_ITEM(10)
      _USER_ACTION(10)
    #endif
    #if HAS_USER_ITEM(11)
      _USER_ACTION(11)
    #endif
    #if HAS_USER_ITEM(12)
      _USER_ACTION(12)
    #endif
    #if HAS_USER_ITEM(13)
      _USER_ACTION(13)
    #endif
    #if HAS_USER_ITEM(14)
      _USER_ACTION(14)
    #endif
    #if HAS_USER_ITEM(15)
      _USER_ACTION(15)
    #endif
    #if HAS_USER_ITEM(16)
      _USER_ACTION(16)
    #endif
    #if HAS_USER_ITEM(17)
      _USER_ACTION(17)
    #endif
    #if HAS_USER_ITEM(18)
      _USER_ACTION(18)
    #endif
    #if HAS_USER_ITEM(19)
      _USER_ACTION(19)
    #endif
    #if HAS_USER_ITEM(20)
      _USER_ACTION(20)
    #endif

    case 1: GOTO_PREVIOUS(); break;
    default: return false;
  }
  return true;
}

#endif // FTDI_CUSTOM_USER_MENUS
