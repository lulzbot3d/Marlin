/************************
 * western_european.cpp *
 ************************/

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

#if defined(FTDI_EXTENDED) && defined(TOUCH_UI_USE_UTF8) && defined(TOUCH_UI_UTF8_WESTERN_CHARSET)

  #include "western_european_bitmap_31.h"

  #define NUM_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

  constexpr static uint8_t std_font = 31;
  constexpr static uint8_t alt_font = 1;

  /* Glyphs in the Western European bitmap */

  enum {
    GRAVE,
    ACUTE,
    CIRCUMFLEX,
    TILDE,
    DIAERESIS,
    DOT_ABOVE,
    CEDILLA,
    NO_DOT_I,
    SHARP_S,
    LRG_O_STROKE,
    SML_O_STROKE,
    LRG_AE,
    SML_AE,
    LRG_ETH,
    SML_ETH,
    LRG_THORN,
    SML_THORN,
    LEFT_DBL_QUOTE,
    RIGHT_DBL_QUOTE,
    INV_EXCLAMATION,
    INV_QUESTION,
    CENT_SIGN,
    POUND_SIGN,
    CURRENCY_SIGN,
    YEN_SIGN
  };

  /* Centerline of characters that can take accents */

  constexpr int8_t mid_a = 12;
  constexpr int8_t mid_e = 12;
  constexpr int8_t mid_i = 5;
  constexpr int8_t mid_o = 12;
  constexpr int8_t mid_u = 12;
  constexpr int8_t mid_y = 11;
  constexpr int8_t mid_n = 12;
  constexpr int8_t mid_c = 12;
  constexpr int8_t mid_A = 13;
  constexpr int8_t mid_E = 13;
  constexpr int8_t mid_I = 6;
  constexpr int8_t mid_O = 14;
  constexpr int8_t mid_U = 14;
  constexpr int8_t mid_Y = 13;
  constexpr int8_t mid_N = 15;
  constexpr int8_t mid_C = 13;

  /* Centerline of accent glyphs */

  constexpr int8_t mid_accent = 16;

  /* When reusing the DOT_ABOVE accent glyph for the degree sign, we need to trim the leading space */
  constexpr uint8_t deg_sign_leading = 8;

  /* Look-up table for constructing characters (must be ordered by unicode)
   *
   * Characters are either complete symbols from the Western European bitmap,
   * or they are constructed using a standard letter from the romfont and
   * drawing an accent from the Western European bitmap over it.
   */

  PROGMEM const struct {
    uint16_t unicode;
    uint8_t  std_char; // Glyph from standard ROMFONT (zero if none)
    uint8_t  alt_char; // Glyph from Western European bitmap
    uint8_t  alt_data; // For accented characters, the centerline; else char width
  } char_recipe[] = {
    { 0,   0,  NO_DOT_I,           10   },
    {'¡',  0 , INV_EXCLAMATION,    13   },
    {'¢',  0 , CENT_SIGN,          23   },
    {'£',  0 , POUND_SIGN,         24   },
    {'¤',  0 , CURRENCY_SIGN,      26   },
    {'¥',  0 , YEN_SIGN,           26   },
    {'«',  0 , LEFT_DBL_QUOTE,     23   },
    {'°',  0 , DOT_ABOVE,          24   },
    {'»',  0 , RIGHT_DBL_QUOTE,    24   },
    {'¿',  0 , INV_QUESTION,       21   },
    {'À', 'A', GRAVE,              mid_A},
    {'Á', 'A', ACUTE,              mid_A},
    {'Â', 'A', CIRCUMFLEX,         mid_A},
    {'Ã', 'A', TILDE,              mid_A},
    {'Ä', 'A', DIAERESIS,          mid_A},
    {'Å', 'A', DOT_ABOVE,          mid_A},
    {'Æ',  0 , LRG_AE,             mid_E},
    {'Ç', 'C', CEDILLA,            mid_C},
    {'È', 'E', GRAVE,              mid_E},
    {'É', 'E', ACUTE,              mid_E},
    {'Ê', 'E', CIRCUMFLEX,         mid_E},
    {'Ë', 'E', DIAERESIS,          mid_E},
    {'Ì', 'I', GRAVE,              mid_I},
    {'Í', 'I', ACUTE,              mid_I},
    {'Î', 'I', CIRCUMFLEX,         mid_I},
    {'Ï', 'I', DIAERESIS,          mid_I},
    {'Ð',  0,  LRG_ETH,            31   },
    {'Ñ', 'N', TILDE,              mid_N},
    {'Ò', 'O', GRAVE,              mid_O},
    {'Ó', 'O', ACUTE,              mid_O},
    {'Ô', 'O', CIRCUMFLEX,         mid_O},
    {'Õ', 'O', TILDE,              mid_O},
    {'Ö', 'O', DIAERESIS,          mid_O},
    {'Ø',  0 , LRG_O_STROKE,       32   },
    {'Ù', 'U', GRAVE,              mid_U},
    {'Ú', 'U', ACUTE,              mid_U},
    {'Û', 'U', CIRCUMFLEX,         mid_U},
    {'Ü', 'U', DIAERESIS,          mid_U},
    {'Ý', 'Y', ACUTE,              mid_Y},
    {'Þ',  0 , LRG_THORN,          25   },
    {'ß',  0 , SHARP_S,            26   },
    {'à', 'a', GRAVE,              mid_a},
    {'á', 'a', ACUTE,              mid_a},
    {'â', 'a', CIRCUMFLEX,         mid_a},
    {'ã', 'a', TILDE,              mid_a},
    {'ä', 'a', DIAERESIS,          mid_a},
    {'å', 'a', DOT_ABOVE,          mid_a},
    {'æ',  0 , SML_AE,             40   },
    {'ç', 'c', CEDILLA,            mid_c},
    {'è', 'e', GRAVE,              mid_e},
    {'é', 'e', ACUTE,              mid_e},
    {'ê', 'e', CIRCUMFLEX,         mid_e},
    {'ë', 'e', DIAERESIS,          mid_e},
    {'ì', 'i', GRAVE,              mid_i},
    {'í', 'i', ACUTE,              mid_i},
    {'î', 'i', CIRCUMFLEX,         mid_i},
    {'ï', 'i', DIAERESIS,          mid_i},
    {'ð',  0,  SML_ETH,            24   },
    {'ñ', 'n', TILDE,              mid_n},
    {'ò', 'o', GRAVE,              mid_o},
    {'ó', 'o', ACUTE,              mid_o},
    {'ô', 'o', CIRCUMFLEX,         mid_o},
    {'õ', 'o', TILDE,              mid_o},
    {'ö', 'o', DIAERESIS,          mid_o},
    {'ø',  0 , SML_O_STROKE,       25   },
    {'ù', 'u', GRAVE,              mid_u},
    {'ú', 'u', ACUTE,              mid_u},
    {'û', 'u', CIRCUMFLEX,         mid_u},
    {'ü', 'u', DIAERESIS,          mid_u},
    {'ý', 'y', ACUTE,              mid_y},
    {'þ',  0 , SML_THORN,          25   },
    {'ÿ', 'y', DIAERESIS,          mid_y}
  };

  /* Performs a binary search to find a unicode character in the table */

  static int8_t find_char_data(FTDI::utf8_char_t c) {
    int8_t min = 0, max = NUM_ELEMENTS(char_recipe), index;
    for (;;) {
      index = (min + max)/2;
      const uint16_t char_at = pgm_read_word(&char_recipe[index].unicode);
      if (char_at == c) break;
      if (min == max) return -1;
      if (c > char_at)
        min = index + 1;
      else
        max = index;
    }
    return index;
  }

  static void get_char_data(uint8_t index, uint8_t &std_char, uint8_t &alt_char, uint8_t &alt_data) {
    std_char = pgm_read_byte(&char_recipe[index].std_char);
    alt_char = pgm_read_byte(&char_recipe[index].alt_char);
    alt_data = pgm_read_byte(&char_recipe[index].alt_data);
  }

  /**
   * Load bitmap data into RAMG. This function is called once at the start
   * of the program.
   *
   * Parameters:
   *
   *   addr  - Address in RAMG where the font data is written
   */

  void FTDI::WesternEuropean::load_data(uint16_t addr) {
    // Load the alternative font metrics
    CLCD::FontMetrics alt_fm;
    alt_fm.ptr    = addr + 148;
    alt_fm.format = L4;
    alt_fm.stride = 19;
    alt_fm.width  = 38;
    alt_fm.height = 49;
    for (uint8_t i = 0; i < 127; i++) {
      alt_fm.char_widths[i] = 0;
    }
    // For special characters, copy the character widths from the char tables
    for (uint8_t i = 0; i < NUM_ELEMENTS(char_recipe); i++) {
      uint8_t std_char, alt_char, alt_data;
      get_char_data(i, std_char, alt_char, alt_data);
      if (std_char == 0)
        alt_fm.char_widths[alt_char] = alt_data;
    }
    CLCD::mem_write_bulk(addr, &alt_fm,  148);

    // Decode the RLE data and load it into RAMG as a bitmap
    write_rle_data(addr + 148, font, sizeof(font));
  }

  /**
   * Populates the bitmap handles for the custom into the display list.
   * This function is called once at the start of each display list.
   *
   * Parameters:
   *
   *   cmd  - Object used for writing to the FTDI chip command queue.
   */

  void FTDI::WesternEuropean::load_bitmaps(CommandProcessor& cmd) {
    CLCD::FontMetrics alt_fm;
    alt_fm.ptr    = 148;
    alt_fm.format = L4;
    alt_fm.stride = 19;
    alt_fm.width  = 38;
    alt_fm.height = 49;
    set_font_bitmap(cmd, alt_fm, alt_font);
  }

  /**
   * Renders a character at location x and y. The x position is incremented
   * by the width of the character.
   *
   * Parameters:
   *
   *   cmd  - If non-NULL the symbol is drawn to the screen.
   *          If NULL, only increment position for text measurement.
   *
   *   x, y - The location at which to draw the character. On output,
   *          incremented to the location of the next character.
   *
   *   fs   - A scaling object used to scale the font. The display will
   *          already be configured to scale bitmaps, but positions
   *          must be scaled using fs.scale()
   *
   *   c    - The unicode code point to draw. If the renderer does not
   *          support the character, it should return false.

   * Returns: Whether the character was supported.
   */

  bool FTDI::WesternEuropean::render_glyph(CommandProcessor* cmd, int &x, int &y, font_size_t fs, utf8_char_t c) {

    // Check to see whether this is a character we support
    if (c < '¡' || c > 'ÿ') return false;

    int8_t index = find_char_data(c);
    if (index == -1) return false;

    // Determine character characteristics
    uint8_t std_char, alt_char, alt_data;
    get_char_data(index, std_char, alt_char, alt_data);

    bool    base_special;
    uint8_t base_width;
    uint8_t base_char;
    uint8_t accent_char;
    int8_t  accent_dx, accent_dy;

    if (std_char == 0) {
      // Special character, non-accented
      base_width   = alt_data;
      base_special = true;
      base_char    = alt_char;
      accent_char  = 0;
      if (c == '°') x -= fs.scale(deg_sign_leading);
    } else {
      // Regular character with accent:
      accent_dx   = alt_data - mid_accent;
      accent_dy   = isupper(std_char) ? -7 : 0;
      accent_char = alt_char;
      base_width  = StandardCharSet::std_char_width(std_char);
      if (std_char == 'i') {
        base_special = true;
        base_char    = NO_DOT_I;
      } else {
        base_special = false;
        base_char    = std_char;
      }
    }

    // If cmd != NULL, draw the glyph to the screen
    if(cmd) {
      if (base_special)
        cmd->cmd(VERTEX2II(x, y, alt_font, base_char));
      else
        cmd->cmd(VERTEX2II(x, y, std_font, base_char));
      if (accent_char)
        cmd->cmd(VERTEX2II(x + fs.scale(accent_dx), y + fs.scale(accent_dy), alt_font, accent_char));
    }

    // Increment X to the next character position
    x += fs.scale(base_width);
    return true;
  }

#endif // FTDI_EXTENDED