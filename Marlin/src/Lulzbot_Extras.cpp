/**********************
 * Lulzbot_Extras.cpp *
 **********************/

/****************************************************************************
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

#include "Marlin.h"

#include "Lulzbot_Extras.h"

/******************************** EMI MITIGATION *******************************/

#if ENABLED(LULZBOT_EMI_MITIGATION)
  void emi_shutoff_pins() {
    #if ENABLED(LULZBOT_EMI_MITIGATION)
      enable_emi_pins(false);

      #define LULZBOT_EMI_SHUTOFF(pin)  SET_OUTPUT(pin); WRITE(pin, LOW);

      #if MB(ARCHIM2)
        LULZBOT_EMI_SHUTOFF(GPIO_PB1_J20_5)
        LULZBOT_EMI_SHUTOFF(GPIO_PB0_J20_6)
        LULZBOT_EMI_SHUTOFF(GPIO_PB3_J20_7)
        LULZBOT_EMI_SHUTOFF(GPIO_PB2_J20_8)
        LULZBOT_EMI_SHUTOFF(GPIO_PB6_J20_9)
        LULZBOT_EMI_SHUTOFF(GPIO_PB5_J20_10)
        LULZBOT_EMI_SHUTOFF(GPIO_PB8_J20_11)
        LULZBOT_EMI_SHUTOFF(GPIO_PB4_J20_12)
        LULZBOT_EMI_SHUTOFF(GPIO_PB9_J20_13)
        LULZBOT_EMI_SHUTOFF(GPIO_PB7_J20_14)
        LULZBOT_EMI_SHUTOFF(GPIO_PB14_J20_17)
        LULZBOT_EMI_SHUTOFF(GPIO_PA18_J20_21)
        LULZBOT_EMI_SHUTOFF(GPIO_PA17_J20_22)
      #endif
    #endif
  }

  #define LULZBOT_SET_PIN_STATE(pin, enable) \
    if(enable) { \
      /* Set as inputs with pull-up resistor */ \
      SET_INPUT(pin); \
      WRITE(pin, HIGH); \
    } else { \
      SET_OUTPUT(pin); \
      WRITE(pin, LOW); \
    }

  /* Enable the probe pins only only when homing/probing,
   * as this helps reduce EMI by grounding the lines.
   *
   * On Mini:
   *   Z_MIN_PIN are the bed washers.
   *
   * On TAZ:
   *   Z_MIN_PIN corresponds to the Z-Home push button.
   *   Z_MIN_PROBE_PIN are the bed washers.
   */
  void enable_emi_pins(const bool enable) {
    #if HAS_BED_PROBE
      LULZBOT_SET_PIN_STATE(Z_MIN_PIN, enable);
      #if PIN_EXISTS(Z_MIN_PROBE)
        LULZBOT_SET_PIN_STATE(Z_MIN_PROBE_PIN, enable)
      #endif
    #endif

    /* Wait to charge up the lines */
    if(enable) delay(5);
  }
#endif

/******************************** EXTRA FEATURES *******************************/

#if ENABLED(EXTENSIBLE_UI)
  #include "lcd/extensible_ui/lib/lulzbot/ftdi_eve_lib/basic/ftdi_basic.h"

  void on_reflash() {
    /* Turn off LCD prior to initiating flash on TAZ Pro */
    CLCD::set_brightness(0);
  }
#endif

/******************************** PROBE QUALITY CHECK *************************/

#if ENABLED(AUTO_BED_LEVELING_LINEAR)
  #include "libs/vector_3.h"

  void BedLevelingReport::report() {
    vector_3 bp[4] = {
      vector_3(x[0],y[0],z[0]),
      vector_3(x[1],y[1],z[1]),
      vector_3(x[2],y[2],z[2])
    };
    vector_3 norm = vector_3::cross(bp[0]-bp[1],bp[1]-bp[2]);
    float a = norm.x, b = norm.y, c = norm.z, d = -bp[0].x*a -bp[0].y*b -bp[0].z*c;
    float dist = abs(a * bp[3].x + b * bp[3].y + c * bp[3].z + d)/sqrt( a*a + b*b + c*c );
    SERIAL_ECHOPAIR("4th probe point, distance from plane: ", dist);
    SERIAL_EOL();
  }
#endif

/*************************** Z-OFFSET AUTO-SAVE  ********************************/

#if HAS_Z_AUTO_SAVE
  #include "HAL/shared/persistent_store_api.h"
  #include "module/probe.h"

  int AutoSaveZOffset::eeprom_offset = -1;

  void AutoSaveZOffset::store() {
    if(eeprom_offset > 0) {
      uint16_t crc;
      persistentStore.write_data(eeprom_offset, (uint8_t*)&zprobe_zoffset, sizeof(zprobe_zoffset), &crc);
      SERIAL_ECHO_START();
      SERIAL_ECHOPAIR("Updating zoffset in EEPROM: ", zprobe_zoffset);
      SERIAL_ECHOPAIR("; EEPROM Index: ", eeprom_offset);
      SERIAL_ECHOLNPGM("");
    }
  }
#endif

/***************************** CUSTOM SPLASH SCREEN *****************************/

#if ENABLED(LULZBOT_ENHANCED_BOOTSCREEN)
  #include "lcd/dogm/fontdata/fontdata_ISO10646_1.h"
  #include "lcd/dogm/ultralcd_DOGM.h"
  #include "lcd/dogm/u8g_fontutf8.h"
  #include "lcd/ultralcd.h"
  #include "lcd/dogm/dogm_Bootscreen.h"

  void MarlinUI::draw_custom_bootscreen(const uint8_t) {
    u8g.drawBitmapP(0,0,CEILING(CUSTOM_BOOTSCREEN_BMPWIDTH, 8),CUSTOM_BOOTSCREEN_BMPHEIGHT,custom_start_bmp);
    u8g.setFont(u8g_font_6x13);
    u8g.drawStr(57,17,LULZBOT_LCD_MACHINE_NAME);
    u8g.setFont(u8g_font_04b_03);
    u8g.drawStr(58,28,LULZBOT_LCD_TOOLHEAD_NAME);
    u8g.setFont(u8g_font_5x8);
    u8g.drawStr(59,41,"LulzBot.com");
    u8g.setFont(u8g_font_5x8);
    u8g.drawStr(61,62,"v");
    u8g.drawStr(66,62,SHORT_BUILD_VERSION);
    u8g.setFont(MENU_FONT_NAME);
  }
#endif