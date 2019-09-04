/***********************
 * font_downloader.ino *
 ***********************/

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

// Dumps a ROM font from the FTDI chip to a XPM file, where it can be
// loaded into an image editor to be examined or modified.

constexpr uint32_t font = 32;
constexpr uint32_t baud = 115200;

#include "src/ftdi_eve_lib/ftdi_eve_lib.h"

using namespace FTDI;

class XPMWriter {
  public:
    uint16_t width;
    uint16_t height;
    uint16_t row;
    uint8_t colors;

  public:
    XPMWriter(int w, int h, int c) : width(w), height(h), colors(c) {
    }

    void write_header() {
      char str[256];

      #define PRINT(...) sprintf(str, __VA_ARGS__); Serial.println(str);

      PRINT("/* XPM */");
      PRINT("static char * FONT[] = {");
      PRINT("/* <Values> */");
      PRINT("/* <width/columns> <height/rows> <colors> <chars per pixels>*/");
      PRINT("\"%d %d %d %d\",", width, height, colors, 1);
      PRINT("/* <Colors> */");

      if(colors == 2) {
        PRINT("\"0 c #FFFFFF\",");
        PRINT("\"1 c #000000\",");
      } else {
        PRINT("\"0 c #FFFFFF\",");
        PRINT("\"1 c #EEEEEE\",");
        PRINT("\"2 c #DDDDDD\",");
        PRINT("\"3 c #CCCCCC\",");
        PRINT("\"4 c #BBBBBB\",");
        PRINT("\"5 c #AAAAAA\",");
        PRINT("\"6 c #999999\",");
        PRINT("\"7 c #888888\",");
        PRINT("\"8 c #777777\",");
        PRINT("\"9 c #666666\",");
        PRINT("\"A c #555555\",");
        PRINT("\"B c #444444\",");
        PRINT("\"C c #333333\",");
        PRINT("\"D c #222222\",");
        PRINT("\"E c #111111\",");
        PRINT("\"F c #000000\",");
      }

      PRINT("/* <Pixels> */");
      row = 0;
    }

    void write_color(uint8_t c) {
      if(row == 0) {
        Serial.print("\"");
      }
      Serial.print(c, HEX);
      if(++row == width) {
        Serial.print("\",\n");
        row = 0;
      }
    }

    void write_footer() {
      if(row != 0) {
        Serial.print("\"\n");
      }
      Serial.println("};");
    }

    void write_comment(char *str) {
      if(row != 0) {
        Serial.print("\",\n");
      }
      Serial.print("/* ");
      Serial.print(str);
      Serial.print(" */\n");
      row = 0;
    }
};

void dump_font_metrics(CLCD::FontMetrics &fm) {
  Serial.print("static uint8_t char_width[] PROGMEM = {");
  for(int i = 0; i < 128; i++) {
    Serial.print(fm.char_widths[i]);
    if(i != 127) Serial.print(", ");
  }
  Serial.println("};");
}

void dump_font_heights() {
  Serial.print("static uint8_t char_heights[] PROGMEM = {");
  for(int font = 16; font <= 34; font++) {
    CLCD::FontMetrics fm(font);
    Serial.print(fm.height);
    if(font != 34) Serial.print(", ");
  }
  Serial.println("};");
}


void dump_font_bitmap(CLCD::FontMetrics &fm) {
  int colors, pixels_per_byte;
  switch(fm.format) {
    case L1: colors = 2;  pixels_per_byte = 8; break;
    case L4: colors = 16; pixels_per_byte = 2; break;
  }

  uint8_t glyph_count = 0;
  for(int i = 0; i < 128; i++) {
    if(fm.char_widths[i] != 0) {
      glyph_count++;
    }
  }

  uint32_t p = fm.ptr;

  const uint32_t glyph_bytes = fm.stride * fm.height;

  XPMWriter xpm(fm.stride * pixels_per_byte, fm.height * glyph_count, colors);
  xpm.write_header();

  for(int i = 0; i < 128; i++) {
    if(fm.char_widths[i] == 0) {
      // Skip the glyph
      p += glyph_bytes;
    } else {
      // Print the glyph
      char str[256];
      sprintf(str, isprint(i) ? "%d %c" : "%d ", i, char(i));
      xpm.write_comment(str);
      for(size_t i = 0; i < glyph_bytes; i++) {
        uint8_t byte = CLCD::mem_read_8(p++);
        if(colors == 16) {
          xpm.write_color(byte >> 0x4);
          xpm.write_color(byte &  0xF);
        } else {
          xpm.write_color(bool(byte & 128));
          xpm.write_color(bool(byte &  64));
          xpm.write_color(bool(byte &  32));
          xpm.write_color(bool(byte &  16));
          xpm.write_color(bool(byte &   8));
          xpm.write_color(bool(byte &   4));
          xpm.write_color(bool(byte &   2));
          xpm.write_color(bool(byte &   1));
        }
      }
    }
  }
  xpm.write_footer();
}

void setup() {
  Serial.begin(baud);

  CLCD::init();

  CommandProcessor cmd;
  cmd.romfont(33,33);
  cmd.romfont(34,34);

  CLCD::FontMetrics fm(font);
  dump_font_bitmap(fm);
  //dump_font_metrics(fm);
  //dump_font_heights();
}

void loop() {
}
