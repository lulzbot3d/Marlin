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

#if defined(FTDI_EXTENDED) && defined(TOUCH_UI_USE_UTF8)

  #include "western_european_bitmap_32.h"

  #define NUM_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

  constexpr static uint8_t std_font = 32;
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

  constexpr int8_t mid_a = 14;
  constexpr int8_t mid_e = 14;
  constexpr int8_t mid_i = 6;
  constexpr int8_t mid_o = 15;
  constexpr int8_t mid_u = 15;
  constexpr int8_t mid_y = 13;
  constexpr int8_t mid_n = 15;
  constexpr int8_t mid_c = 14;
  constexpr int8_t mid_A = 16;
  constexpr int8_t mid_E = 15;
  constexpr int8_t mid_I = 9;
  constexpr int8_t mid_O = 18;
  constexpr int8_t mid_U = 18;
  constexpr int8_t mid_Y = 16;
  constexpr int8_t mid_N = 20;
  constexpr int8_t mid_C = 17;

  /* Centerline of accent glyphs */

  constexpr int8_t mid_accent = 19;

  /* When reusing the DOT_ABOVE accent glyph for the degree sign, we need to trim the leading space */
  constexpr uint8_t deg_sign_leading = 11;

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
    { 0,   0,  NO_DOT_I,           12   },
    {'¡',  0 , INV_EXCLAMATION,    16   },
    {'¢',  0 , CENT_SIGN,          28   },
    {'£',  0 , POUND_SIGN,         30   },
    {'¤',  0 , CURRENCY_SIGN,      32   },
    {'¥',  0 , YEN_SIGN,           32   },
    {'«',  0 , LEFT_DBL_QUOTE,     29   },
    {'°',  0 , DOT_ABOVE,          29   },
    {'»',  0 , RIGHT_DBL_QUOTE,    30   },
    {'¿',  0 , INV_QUESTION,       25   },
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
    {'Ð',  0,  LRG_ETH,            39   },
    {'Ñ', 'N', TILDE,              mid_N},
    {'Ò', 'O', GRAVE,              mid_O},
    {'Ó', 'O', ACUTE,              mid_O},
    {'Ô', 'O', CIRCUMFLEX,         mid_O},
    {'Õ', 'O', TILDE,              mid_O},
    {'Ö', 'O', DIAERESIS,          mid_O},
    {'Ø',  0 , LRG_O_STROKE,       40   },
    {'Ù', 'U', GRAVE,              mid_U},
    {'Ú', 'U', ACUTE,              mid_U},
    {'Û', 'U', CIRCUMFLEX,         mid_U},
    {'Ü', 'U', DIAERESIS,          mid_U},
    {'Ý', 'Y', ACUTE,              mid_Y},
    {'Þ',  0 , LRG_THORN,          32   },
    {'ß',  0 , SHARP_S,            32   },
    {'à', 'a', GRAVE,              mid_a},
    {'á', 'a', ACUTE,              mid_a},
    {'â', 'a', CIRCUMFLEX,         mid_a},
    {'ã', 'a', TILDE,              mid_a},
    {'ä', 'a', DIAERESIS,          mid_a},
    {'å', 'a', DOT_ABOVE,          mid_a},
    {'æ',  0 , SML_AE,             46   },
    {'ç', 'c', CEDILLA,            mid_c},
    {'è', 'e', GRAVE,              mid_e},
    {'é', 'e', ACUTE,              mid_e},
    {'ê', 'e', CIRCUMFLEX,         mid_e},
    {'ë', 'e', DIAERESIS,          mid_e},
    {'ì', 'i', GRAVE,              mid_i},
    {'í', 'i', ACUTE,              mid_i},
    {'î', 'i', CIRCUMFLEX,         mid_i},
    {'ï', 'i', DIAERESIS,          mid_i},
    {'ð',  0,  SML_ETH,            31   },
    {'ñ', 'n', TILDE,              mid_n},
    {'ò', 'o', GRAVE,              mid_o},
    {'ó', 'o', ACUTE,              mid_o},
    {'ô', 'o', CIRCUMFLEX,         mid_o},
    {'õ', 'o', TILDE,              mid_o},
    {'ö', 'o', DIAERESIS,          mid_o},
    {'ø',  0 , SML_O_STROKE,       32   },
    {'ù', 'u', GRAVE,              mid_u},
    {'ú', 'u', ACUTE,              mid_u},
    {'û', 'u', CIRCUMFLEX,         mid_u},
    {'ü', 'u', DIAERESIS,          mid_u},
    {'ý', 'y', ACUTE,              mid_y},
    {'þ',  0 , SML_THORN,          32   },
    {'ÿ', 'y', DIAERESIS,          mid_y}
  };

  /* Performs a binary search to find a unicode character in the table */

  static uint8_t find_char_data(FTDI::utf8_char_t c) {
    uint8_t min = 0, max = NUM_ELEMENTS(char_recipe), index;
    while (min != max) {
      index = (min + max)/2;
      const uint16_t char_at = pgm_read_word(&char_recipe[index].unicode);
      if(char_at == c) break;
      if(c > char_at)
        min = index;
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

  /* Lookup table of the char widths for standard ROMFONT 32 */

  uint8_t std_char_width(char c) {
    static const uint8_t tbl[] PROGMEM = {
      13, 15, 19, 33, 31, 40, 34, 11, 18, 18, 24, 30, 12, 24, 14, 22, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 13, 14, 28, 30, 29, 26, 49, 34, 34, 34, 36, 29, 29, 37,
      37, 15, 30, 34, 29, 46, 37, 37, 34, 38, 33, 33, 32, 37, 34, 46, 34, 34, 32, 15,
      22, 15, 23, 26, 17, 30, 31, 28, 31, 29, 19, 31, 31, 13, 14, 28, 13, 47, 31, 31,
      31, 31, 19, 29, 17, 31, 27, 41, 27, 27, 27, 18, 14, 18, 36, 13
    };
    return pgm_read_byte(&tbl[c - ' ']);
  }

  void FTDI::WesternEuropean::load_data(uint16_t addr) {
    // Load the alternative font metrics
    CLCD::FontMetrics alt_fm;
    alt_fm.ptr    = 148;
    alt_fm.format = L4;
    alt_fm.stride = 23;
    alt_fm.width  = 46;
    alt_fm.height = 63;
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

  void FTDI::WesternEuropean::load_bitmaps(CommandProcessor& cmd) {
    CLCD::FontMetrics alt_fm;
    alt_fm.ptr    = 148;
    alt_fm.format = L4;
    alt_fm.stride = 23;
    alt_fm.width  = 46;
    alt_fm.height = 63;

    CLCD::FontMetrics std_fm(std_font);
    set_font_bitmap(cmd, std_fm, std_font);
    set_font_bitmap(cmd, alt_fm, alt_font);
  }

  void FTDI::WesternEuropean::render_glyph(CommandProcessor* cmd, int &x, int &y, font_size_t fs, utf8_char_t c) {
    bool    base_special;
    uint8_t base_width;
    uint8_t base_char;
    uint8_t accent_char;
    int8_t  accent_dx, accent_dy;

    if (c < 128) {
      // Regular character
      accent_char  = 0;
      base_width   = std_char_width(c);
      base_char    = c;
      base_special = false;
    } else {
      uint8_t std_char, alt_char, alt_data;
      get_char_data(find_char_data(c), std_char, alt_char, alt_data);
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
        accent_dy   = isupper(std_char) ? -10 : 0;
        accent_char = alt_char;
        base_width  = std_char_width(std_char);
        if (std_char == 'i') {
          base_special = true;
          base_char    = NO_DOT_I;
        } else {
          base_special = false;
          base_char    = std_char;
        }
      }
    }

    if(cmd) {
      if (base_special)
        cmd->cmd(VERTEX2II(x, y, alt_font, base_char));
      else
        cmd->cmd(VERTEX2II(x, y, std_font, base_char));
      if (accent_char)
        cmd->cmd(VERTEX2II(x + fs.scale(accent_dx), y + fs.scale(accent_dy), alt_font, accent_char));
    }

    x += fs.scale(base_width);
  }

#endif // FTDI_EXTENDED