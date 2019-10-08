/************
 * colors.h *
 ************/

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

#pragma once

namespace Theme {
  #ifdef LULZBOT_USE_BIOPRINTER_UI
    // Use a blue accent color
    constexpr uint32_t accent_color_1   = 0x004480;  // HSL 208°, 100%, 25% - Darkest
    constexpr uint32_t accent_color_2   = 0x006ac6;  // HSL 208°, 100%, 39% - (color of logo on case)
    constexpr uint32_t accent_color_3   = 0x1a94ff;  // HSL 208°, 100%, 55%
    constexpr uint32_t accent_color_4   = 0x66b8ff;  // HSL 208°, 100%, 70%
    constexpr uint32_t accent_color_5   = 0xb3dbff;  // HSL 208°, 100%, 85% - Lightest

    constexpr uint32_t logo_bg          = 0xffffff;
    constexpr uint32_t logo_fg          = 0xffffff;
    constexpr uint32_t logo_stroke      = accent_color_2;  // HSL 208°, 100%, 39%
  #else
    // Use a green accent color
    constexpr uint32_t accent_color_1   = 0x545923;  // HSL  66°, 44%, 24%  - Darkest
    constexpr uint32_t accent_color_2   = 0x90a215;  // HSL  68°, 77%, 36%
    constexpr uint32_t accent_color_3   = 0xc1d82f;  // HSL  68°, 68%, 52%
    constexpr uint32_t accent_color_4   = 0xdeea5c;  // HSL  65°, 77%, 64%
    constexpr uint32_t accent_color_5   = 0xf1f6c0;  // HSL  66°, 75%, 86%  - Lightest

    constexpr uint32_t logo_bg          = accent_color_4;
    constexpr uint32_t logo_fg          = accent_color_3;
    constexpr uint32_t logo_stroke      = 0x000000;
  #endif

  #ifndef LULZBOT_USE_BIOPRINTER_UI
    constexpr uint32_t theme_darkest    = 0x444433;
    constexpr uint32_t theme_dark       = 0x777759;

    constexpr uint32_t bg_color         = theme_darkest;
    constexpr uint32_t bg_text_disabled = theme_dark;
    constexpr uint32_t bg_text_enabled  = 0xFFFFFF;
    constexpr uint32_t bg_normal        = theme_darkest;

    constexpr uint32_t fg_normal        = theme_dark;
    constexpr uint32_t fg_action        = accent_color_2;
    constexpr uint32_t fg_disabled      = theme_darkest;
  #else
    constexpr uint32_t theme_darkest    = accent_color_1;
    constexpr uint32_t theme_dark       = accent_color_4;

    constexpr uint32_t bg_color         = 0xFFFFFF;
    constexpr uint32_t bg_text_disabled = 0x333333;
    constexpr uint32_t bg_text_enabled  = accent_color_1;
    constexpr uint32_t bg_normal        = accent_color_4;

    constexpr uint32_t fg_normal        = accent_color_1;
    constexpr uint32_t fg_action        = accent_color_4;
    constexpr uint32_t fg_disabled      = 0xEFEFEF;

    constexpr uint32_t shadow_rgb       = 0xE0E0E0;
    constexpr uint32_t stroke_rgb       = accent_color_1;
    constexpr uint32_t fill_rgb         = accent_color_3;
    constexpr uint32_t syringe_rgb      = accent_color_5;
  #endif

  constexpr uint32_t x_axis        = 0xFF0000;
  constexpr uint32_t y_axis        = 0x00BB00;
  constexpr uint32_t z_axis        = 0x0000BF;
  #ifndef LULZBOT_USE_BIOPRINTER_UI
  constexpr uint32_t e_axis        = 0x777759;
  constexpr uint32_t feedrate      = 0x777759;
  constexpr uint32_t other         = 0x777759;
  #else
  constexpr uint32_t e_axis        = 0x000000;
  constexpr uint32_t feedrate      = 0x000000;
  constexpr uint32_t other         = 0x000000;
  #endif

  // Status screen
  constexpr uint32_t progress      = theme_dark;
  constexpr uint32_t status_msg    = theme_dark;
  constexpr uint32_t fan_speed     = 0x377198;
  constexpr uint32_t temp          = 0x892c78;
  constexpr uint32_t axis_label    = theme_dark;

  constexpr uint32_t disabled_icon = 0x101010;

  // Calibration Registers Screen
  constexpr uint32_t transformA    = 0x3010D0;
  constexpr uint32_t transformB    = 0x4010D0;
  constexpr uint32_t transformC    = 0x5010D0;
  constexpr uint32_t transformD    = 0x6010D0;
  constexpr uint32_t transformE    = 0x7010D0;
  constexpr uint32_t transformF    = 0x8010D0;
  constexpr uint32_t transformVal  = 0x104010;

  constexpr btn_colors disabled_btn = {.bg = bg_color,      .grad = fg_disabled, .fg = fg_disabled,  .rgb = fg_disabled };
  constexpr btn_colors normal_btn   = {.bg = fg_action,     .grad = 0xFFFFFF,    .fg = fg_normal,    .rgb = 0xFFFFFF };
  constexpr btn_colors action_btn   = {.bg = bg_color,      .grad = 0xFFFFFF,    .fg = fg_action,    .rgb = 0xFFFFFF };
  constexpr btn_colors red_btn      = {.bg = 0xFF5555,      .grad = 0xFFFFFF,    .fg = 0xFF0000,     .rgb = 0xFFFFFF };
  constexpr btn_colors ui_slider    = {.bg = theme_darkest, .grad = 0xFFFFFF,    .fg = theme_dark,   .rgb = accent_color_3 };
  constexpr btn_colors ui_toggle    = {.bg = theme_darkest, .grad = 0xFFFFFF,    .fg = theme_dark,   .rgb = 0xFFFFFF };

  // Temperature color scale

  const rgb_t cool_rgb (  0,   0,   0);
  const rgb_t low_rgb  (128,   0,   0);
  const rgb_t med_rgb  (255, 128,   0);
  const rgb_t high_rgb (255, 255, 128);
};
