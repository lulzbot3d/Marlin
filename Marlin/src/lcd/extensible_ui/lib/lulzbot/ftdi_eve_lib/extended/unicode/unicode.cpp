/***************
 * unicode.cpp *
 ***************/

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

#include "../ftdi_extended.h"

#if defined(FTDI_EXTENDED) && defined(TOUCH_UI_USE_UTF8)

  using namespace FTDI;

  typedef WesternEuropean Language;

  /* Returns the next character in a UTF8 string and increments the
   * pointer to the next character */

  utf8_char_t FTDI::get_utf8_char_and_inc(const char *&c) {
    if (*c <= 0b01111111)
      // U+0000 through U+007F; single byte
      return *c++;
    else if (*c <= 0b1101111)
      // U+0080 through U+07FF; two bytes
      return ((*c++) & 0b00011111) << 6 | (*c++) & 0b00111111;
    else
      // Additional three and four byte characters not currently supported.
      return 0;
  }

  /* Loads the bitmap data for unicode rendering into RAMG at the specified address */

  void FTDI::load_utf8_data(uint16_t addr) {
    Language::load_data(addr);
  }

  /* Sets up the bitmap handles for drawing the fonts */

  void FTDI::load_utf8_bitmaps(CommandProcessor &cmd) {
    Language::load_bitmaps(cmd);
  }

  /* Helper function for drawing and/or measuring UTF8 text */

  static uint16_t render_utf8_text(CommandProcessor* cmd, int x, int y, const char *str, font_size_t fs) {
    const int start_x = x;
    while (*str) {
      const utf8_char_t c = get_utf8_char_and_inc(str);
      Language::render_glyph(cmd, x, y, fs, c);
    }
    return x - start_x;
  }

  /* Measures an utf8 text string */

  uint16_t FTDI::get_utf8_char_width(utf8_char_t c, font_size_t fs) {
    int x = 0, y = 0;
    Language::render_glyph(NULL, x, y, fs, c);
    return x;
  }

  uint16_t FTDI::get_utf8_text_width(const char *str, font_size_t fs) {
    return render_utf8_text(NULL, 0, 0, str, fs);
  }

  uint16_t FTDI::get_utf8_text_width(progmem_str pstr, font_size_t fs) {
    char str[strlen_P((const char*)pstr) + 1];
    strcpy_P(str, (const char*)pstr);
    return get_utf8_text_width((const char*) pstr, fs);
  }

  /* Draws an utf8 text string */

  void FTDI::draw_utf8_text(CommandProcessor& cmd, int x, int y, const char *str, font_size_t fs, uint16_t options = 0) {
    cmd.cmd(SAVE_CONTEXT());
    cmd.cmd(BITMAP_TRANSFORM_A(fs.get_coefficient()));
    cmd.cmd(BITMAP_TRANSFORM_E(fs.get_coefficient()));
    cmd.cmd(BEGIN(BITMAPS));
    if (options & OPT_CENTERX)
      x -= get_utf8_text_width(str, fs) / 2;
    else if (options & OPT_RIGHTX)
      x -= get_utf8_text_width(str, fs);
    if(options & OPT_CENTERY)
      y -= fs.get_height()/2;
    render_utf8_text(&cmd, x, y, str, fs);
    cmd.cmd(RESTORE_CONTEXT());
  }

  void FTDI::draw_utf8_text(CommandProcessor& cmd, int x, int y, progmem_str pstr, font_size_t fs, uint16_t options = 0) {
    char str[strlen_P((const char*)pstr) + 1];
    strcpy_P(str, (const char*)pstr);
    draw_utf8_text(cmd, x, y, (const char*) str, fs, options);
  }

#endif // FTDI_EXTENDED