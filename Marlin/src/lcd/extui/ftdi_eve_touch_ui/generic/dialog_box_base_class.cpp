/*****************************
 * dialog_box_base_class.cpp *
 *****************************/

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
 *   location: <https://www.gnu.org/licenses/>.                             *
 ****************************************************************************/

#include "../config.h"
#include "../screens.h"

#ifdef FTDI_DIALOG_BOX_BASE_CLASS

using namespace FTDI;
using namespace Theme;
using namespace ExtUI;

#define GRID_COLS 2
#define GRID_ROWS 8

template<typename T>
void DialogBoxBaseClass::drawMessage(T message, const int16_t font) {
  CommandProcessor cmd;
  cmd.cmd(CMD_DLSTART)
     .cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);
  draw_text_box(cmd, BTN_POS(1,1), BTN_SIZE(2,6), message, OPT_CENTER, font ?: font_large);
  cmd.colors(normal_btn);
}

template void DialogBoxBaseClass::drawMessage(PGM_P const, const int16_t);

void DialogBoxBaseClass::drawYesNoButtons(uint8_t default_btn) {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .colors(default_btn == 1 ? action_btn : action_btn).tag(1).button(BTN_POS(2,8), BTN_SIZE(1,1), GET_TEXT_F(MSG_YES))
     .colors(default_btn == 2 ? action_btn : normal_btn).tag(2).button(BTN_POS(1,8), BTN_SIZE(1,1), GET_TEXT_F(MSG_NO));
}

void DialogBoxBaseClass::_format_time(char *outstr, uint32_t time) {
  const uint8_t hrs = time / 3600,
                min = (time / 60) % 60,
                sec = time % 60;
  if (hrs)
    sprintf_P(outstr, PSTR("%02d:%02d"), hrs, min);
  else
    sprintf_P(outstr, PSTR("%02d:%02ds"), min, sec);
}

void DialogBoxBaseClass::drawReprintButtons(uint8_t default_btn) {
  #undef GRID_COLS
  #undef GRID_ROWS
  #define GRID_COLS 6
  #define GRID_ROWS 16

  #define TEMP_RECT_E0        BTN_POS(1,1),  BTN_SIZE(3,2)
  #define TEMP_RECT_E1        BTN_POS(4,1),  BTN_SIZE(3,2)
  #define TEMP_RECT_BED       BTN_POS(1,3),  BTN_SIZE(3,2)
  #define NOZ_1_POS           BTN_POS(1,1),  BTN_SIZE(3,2)
  #define NOZ_2_POS           BTN_POS(4,1),  BTN_SIZE(3,2)
  #define BED_POS             BTN_POS(1,3),  BTN_SIZE(3,2)
  #define FAN_POS             BTN_POS(4,3),  BTN_SIZE(3,2)
  #define ALL_TEMP_POS        BTN_POS(1,1),  BTN_SIZE(6,4)

  #define _ICON_POS(x,y,w,h) x, y, w/3, h
  #define _TEXT_POS(x,y,w,h) x + w/3, y, w - w/3, h
  #define ICON_POS(pos) _ICON_POS(pos)
  #define TEXT_POS(pos) _TEXT_POS(pos)

  CommandProcessor cmd;

  const uint32_t elapsed = getProgress_seconds_elapsed();
  char elapsed_str[10];
  _format_time(elapsed_str, elapsed);

  cmd.cmd(CMD_DLSTART)
     .cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);
  draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(6,2), GET_TEXT_F(MSG_PRINT_COMPLETE), OPT_CENTER, font_xlarge);
  draw_text_box(cmd, BTN_POS(1,7), BTN_SIZE(6,3), GET_TEXT_F(MSG_PRINT_CLEAR_FILAMENT), OPT_CENTER, font_large);
  draw_text_box(cmd, BTN_POS(1,10), BTN_SIZE(3,2), GET_TEXT_F(MSG_INFO_PRINT_TIME), OPT_RIGHTX | OPT_CENTERY, font_large);
  draw_text_box(cmd, BTN_POS(4,10), BTN_SIZE(3,2), elapsed_str, OPT_CENTER, font_large);
  cmd.font(font_large)
     .colors(normal_btn).tag(1).button(BTN_POS(1,13), BTN_SIZE(6,2), GET_TEXT_F(MSG_REPRINT_LAST_JOB))
     .colors(action_btn).tag(2).button(BTN_POS(1,15), BTN_SIZE(6,2), GET_TEXT_F(MSG_MAIN_MENU));

  // Draw Extruder Bitmap on Extruder Temperature Button
  cmd.font(Theme::font_small).tag(5)
      .cmd (BITMAP_SOURCE(Extruder_Icon_Info))
      .cmd (BITMAP_LAYOUT(Extruder_Icon_Info))
      .cmd (BITMAP_SIZE  (Extruder_Icon_Info))
      .icon(ICON_POS(NOZ_1_POS), Extruder_Icon_Info, icon_scale)
      .icon(ICON_POS(NOZ_2_POS), Extruder_Icon_Info, icon_scale);

  // Draw Bed Heat Bitmap on Bed Heat Button
  cmd.cmd (BITMAP_SOURCE(Bed_Heat_Icon_Info))
      .cmd (BITMAP_LAYOUT(Bed_Heat_Icon_Info))
      .cmd (BITMAP_SIZE  (Bed_Heat_Icon_Info))
      .icon(ICON_POS(BED_POS), Bed_Heat_Icon_Info, icon_scale);

  // Draw Fan Percent Bitmap on Bed Heat Button
  cmd.cmd (BITMAP_SOURCE(Fan_Icon_Info))
      .cmd (BITMAP_LAYOUT(Fan_Icon_Info))
      .cmd (BITMAP_SIZE  (Fan_Icon_Info))
      .icon(ICON_POS(FAN_POS), Fan_Icon_Info, icon_scale);

  TERN_(TOUCH_UI_USE_UTF8, load_utf8_bitmaps(cmd)); // Restore font bitmap handles

  char e0_str[20], e1_str[20], bed_str[20], fan_str[20];

  sprintf_P(fan_str, PSTR("%-3d %%"), int8_t(getActualFan_percent(FAN0)));

  if (isHeaterIdle(BED))
    format_temp_and_idle(bed_str, getActualTemp_celsius(BED));
  else
    format_temp_and_temp(bed_str, getActualTemp_celsius(BED), getTargetTemp_celsius(BED));

  if (isHeaterIdle(H0))
    format_temp_and_idle(e0_str, getActualTemp_celsius(H0));
  else
    format_temp_and_temp(e0_str, getActualTemp_celsius(H0), getTargetTemp_celsius(H0));

  #if HAS_MULTI_EXTRUDER
    if (isHeaterIdle(H1))
      format_temp_and_idle(e1_str, getActualTemp_celsius(H1));
    else
      format_temp_and_temp(e1_str, getActualTemp_celsius(H1), getTargetTemp_celsius(H1));
  #else
    strcpy_P(e1_str, PSTR("N/A"));
  #endif

  if (getTargetTemp_celsius(H0) > 0) {
    cmd.fgcolor(temp_button);
  }
  else{
    cmd.colors(normal_btn);
  }
  cmd.tag(0).font(font_medium).button(TEXT_POS(NOZ_1_POS), e0_str);

  if (getTargetTemp_celsius(BED) > 0) {
    cmd.fgcolor(temp_button);
  }
  else{
    cmd.colors(normal_btn);
  }
  cmd.tag(0).font(font_medium).button(TEXT_POS(BED_POS), bed_str);

  if (getActualFan_percent(FAN0) > 0) {
    cmd.fgcolor(temp_button);
  }
  else{
    cmd.colors(normal_btn);
  }
  cmd.tag(0).font(font_medium).button(TEXT_POS(FAN_POS), fan_str);

  if DISABLED(HAS_MULTI_HOTEND){
    cmd.font(font_xsmall).fgcolor(gray_color_1);
  }
  else if (getTargetTemp_celsius(H1) > 0) {
    cmd.font(font_medium).fgcolor(temp_button);
  }
  else{
    cmd.font(font_medium).colors(normal_btn);
  }
  cmd.tag(1).button(TEXT_POS(NOZ_2_POS), e1_str);

  #undef GRID_COLS
  #undef GRID_ROWS
  #define GRID_COLS 2
  #define GRID_ROWS 8
}

void DialogBoxBaseClass::drawStartPrintButtons(uint8_t default_btn) {
  #undef GRID_COLS
  #undef GRID_ROWS
  #define GRID_COLS 1
  #define GRID_ROWS 13

  CommandProcessor cmd;

  cmd.cmd(CMD_DLSTART)
     .cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true))
     .cmd(COLOR_RGB(bg_text_enabled))
     .tag(0);
  draw_text_box(cmd, BTN_POS(1,2), BTN_SIZE(1,3), GET_TEXT_F(MSG_PRINT_COMPLETE), OPT_CENTER, font_xlarge);
  draw_text_box(cmd, BTN_POS(1,5), BTN_SIZE(1,3), GET_TEXT_F(MSG_PRINT_AREA_CLEAR), OPT_CENTER, font_large);
  cmd.font(font_large)
     .colors(normal_btn).tag(1).button(BTN_POS(1,9), BTN_SIZE(1,2), GET_TEXT_F(MSG_START_NEXT_PRINT))
     .colors(action_btn).tag(2).button(BTN_POS(1,11), BTN_SIZE(1,2), GET_TEXT_F(MSG_MAIN_MENU));

  #undef GRID_COLS
  #undef GRID_ROWS
  #define GRID_COLS 2
  #define GRID_ROWS 8
}

void DialogBoxBaseClass::drawOkayButton() {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button(BTN_POS(1,8), BTN_SIZE(2,1), GET_TEXT_F(MSG_BUTTON_OKAY));
}

void DialogBoxBaseClass::drawDoneButton() {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).colors(action_btn).button(BTN_POS(1,8), BTN_SIZE(2,1), GET_TEXT_F(MSG_BUTTON_DONE));
}

template<typename T>
void DialogBoxBaseClass::drawButton(T label) {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button(BTN_POS(1,8), BTN_SIZE(2,1), label);
}

template void DialogBoxBaseClass::drawButton(const char *);
template void DialogBoxBaseClass::drawButton(FSTR_P);

void DialogBoxBaseClass::drawFilamentButtons() {
  CommandProcessor cmd;
  cmd.font(font_medium)
     .tag(1).button(BTN_POS(1,7), BTN_SIZE(2,1), GET_TEXT_F(MSG_FILAMENT_CHANGE_OPTION_PURGE))
     .tag(2).button(BTN_POS(1,8), BTN_SIZE(2,1), GET_TEXT_F(MSG_FILAMENT_CHANGE_OPTION_RESUME));
}

bool DialogBoxBaseClass::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1: GOTO_SCREEN(StatusScreen); return true;
    case 2: GOTO_SCREEN(StatusScreen); return true;
    default:                 return false;
  }
}

void DialogBoxBaseClass::onIdle() {
  reset_menu_timeout();
}

void DialogBoxBaseClass::draw_temperature() {
  using namespace Theme;

  #define TEMP_RECT_E0        BTN_POS(1,1),  BTN_SIZE(3,2)
  #define TEMP_RECT_E1        BTN_POS(4,1),  BTN_SIZE(3,2)
  #define TEMP_RECT_BED       BTN_POS(1,3),  BTN_SIZE(3,2)
  #define NOZ_1_POS           BTN_POS(1,1),  BTN_SIZE(3,2)
  #define NOZ_2_POS           BTN_POS(4,1),  BTN_SIZE(3,2)
  #define BED_POS             BTN_POS(1,3),  BTN_SIZE(3,2)
  #define FAN_POS             BTN_POS(4,3),  BTN_SIZE(3,2)
  #define ALL_TEMP_POS        BTN_POS(1,1),  BTN_SIZE(6,4)

  #define _ICON_POS(x,y,w,h) x, y, w/3, h
  #define _TEXT_POS(x,y,w,h) x + w/3, y, w - w/3, h
  #define ICON_POS(pos) _ICON_POS(pos)
  #define TEXT_POS(pos) _TEXT_POS(pos)

  CommandProcessor cmd;
    // Draw Extruder Bitmap on Extruder Temperature Button
    /*cmd.font(Theme::font_small).tag(5)
       .cmd (BITMAP_SOURCE(Extruder_Icon_Info))
       .cmd (BITMAP_LAYOUT(Extruder_Icon_Info))
       .cmd (BITMAP_SIZE  (Extruder_Icon_Info))
       .icon(ICON_POS(NOZ_1_POS), Extruder_Icon_Info, icon_scale)
       .icon(ICON_POS(NOZ_2_POS), Extruder_Icon_Info, icon_scale);

    // Draw Bed Heat Bitmap on Bed Heat Button
    cmd.cmd (BITMAP_SOURCE(Bed_Heat_Icon_Info))
       .cmd (BITMAP_LAYOUT(Bed_Heat_Icon_Info))
       .cmd (BITMAP_SIZE  (Bed_Heat_Icon_Info))
       .icon(ICON_POS(BED_POS), Bed_Heat_Icon_Info, icon_scale);

    // Draw Fan Percent Bitmap on Bed Heat Button
    cmd.cmd (BITMAP_SOURCE(Fan_Icon_Info))
       .cmd (BITMAP_LAYOUT(Fan_Icon_Info))
       .cmd (BITMAP_SIZE  (Fan_Icon_Info))
       .icon(ICON_POS(FAN_POS), Fan_Icon_Info, icon_scale);
*/
    //TERN_(TOUCH_UI_USE_UTF8, load_utf8_bitmaps(cmd)); // Restore font bitmap handles

    using namespace ExtUI;
    char e0_str[20], e1_str[20], bed_str[20], fan_str[20];

    sprintf_P(fan_str, PSTR("%-3d %%"), int8_t(getActualFan_percent(FAN0)));

    if (isHeaterIdle(BED))
      format_temp_and_idle(bed_str, getActualTemp_celsius(BED));
    else
      format_temp_and_temp(bed_str, getActualTemp_celsius(BED), getTargetTemp_celsius(BED));

    if (isHeaterIdle(H0))
      format_temp_and_idle(e0_str, getActualTemp_celsius(H0));
    else
      format_temp_and_temp(e0_str, getActualTemp_celsius(H0), getTargetTemp_celsius(H0));

    #if HAS_MULTI_EXTRUDER
      if (isHeaterIdle(H1))
        format_temp_and_idle(e1_str, getActualTemp_celsius(H1));
      else
        format_temp_and_temp(e1_str, getActualTemp_celsius(H1), getTargetTemp_celsius(H1));
    #else
      strcpy_P(e1_str, PSTR("N/A"));
    #endif

    if (getTargetTemp_celsius(H0) > 0) {
      cmd.fgcolor(temp_button);
    }
    else{
      cmd.colors(normal_btn);
    }
    cmd.tag(0).font(font_medium).button(TEXT_POS(NOZ_1_POS), e0_str);

    if (getTargetTemp_celsius(BED) > 0) {
      cmd.fgcolor(temp_button);
    }
    else{
      cmd.colors(normal_btn);
    }
    cmd.tag(0).font(font_medium).button(TEXT_POS(BED_POS), bed_str);

    if (getActualFan_percent(FAN0) > 0) {
      cmd.fgcolor(temp_button);
    }
    else{
      cmd.colors(normal_btn);
    }
    cmd.tag(0).font(font_medium).button(TEXT_POS(FAN_POS), fan_str);

    if DISABLED(HAS_MULTI_HOTEND){
      cmd.font(font_xsmall).fgcolor(gray_color_1);
    }
    else if (getTargetTemp_celsius(H1) > 0) {
      cmd.font(font_medium).fgcolor(temp_button);
    }
    else{
      cmd.font(font_medium).colors(normal_btn);
    }
    cmd.tag(0).button(TEXT_POS(NOZ_2_POS), e1_str);
}

#endif // FTDI_DIALOG_BOX_BASE_CLASS
